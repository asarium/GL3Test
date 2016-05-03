//
//

#include "AssimpModelConverter.hpp"
#include "Model.hpp"

#include <assimp/postprocess.h>
#include <assimp/Logger.hpp>
#include <assimp/DefaultLogger.hpp>
#include <sstream>
#include <unordered_map>
#include <fstream>
#include <jansson.h>
#include <glm/gtc/type_ptr.hpp>

namespace {
uint32_t DEFAULT_POST_PROCESSING_STEPS =
    aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_RemoveComponent
        | aiProcess_GenNormals | aiProcess_SplitLargeMeshes | aiProcess_ValidateDataStructure
        | aiProcess_ImproveCacheLocality | aiProcess_RemoveRedundantMaterials | aiProcess_SortByPType
        | aiProcess_FindDegenerates | aiProcess_FindInvalidData | aiProcess_GenUVCoords | aiProcess_TransformUVCoords
        | aiProcess_FindInstances | aiProcess_OptimizeMeshes;

const int SUPPORTED_VERSION = 1;

bool loggerCreated = false;
void createAILogger() {
    if (loggerCreated) {
        return;
    }
    loggerCreated = true;

    // Change this line to normal if you not want to analyse the import process
    Assimp::Logger::LogSeverity severity = Assimp::Logger::NORMAL;
    //Assimp::Logger::LogSeverity severity = Assimp::Logger::VERBOSE;

    // Create a logger instance for Console Output
    Assimp::DefaultLogger::create("", severity, aiDefaultLogStream_STDOUT);

    // Now I am ready for logging my stuff
    Assimp::DefaultLogger::get()->info("this is my info-call");
}

uint16_t process_index(uint32_t index, std::pair<uint32_t, uint32_t>& min_max_pair) {
    if (index > std::numeric_limits<uint16_t>::max()) {
        throw std::runtime_error("An index had a too large value!");
    }

    min_max_pair.first = std::min(index, min_max_pair.first);
    min_max_pair.second = std::max(index, min_max_pair.second);

    return static_cast<uint16_t>(index);
}

std::string cleanup_path(const char* path) {
#ifdef WIN32
#error Not yet implemented!
#else
    // Assume POSIX
    auto filename = std::string(basename(path));

//    size_t dot_pos = filename.find_last_of('.');
//    if (dot_pos != std::string::npos) {
//        filename.resize(dot_pos); // Remove extension
//    }

    return filename;
#endif
}

ExportMaterial convertAssimpMaterial(aiMaterial* material) {
    ExportMaterial ret_val;

    aiString name;
    if (material->Get(AI_MATKEY_NAME, name) != aiReturn_SUCCESS) {
        ret_val.name = "";
    } else {
        ret_val.name = name.C_Str();
    }

    if (material->GetTextureCount(aiTextureType_DIFFUSE) < 1) {
        throw std::runtime_error("A diffuse texture is required!");
    }
    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 1) {
        throw std::runtime_error("Multiple diffuse textures are not supported!");
    }

    aiString path;
    if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) != aiReturn_SUCCESS) {
        throw std::runtime_error("Failed to get diffuse texture path!");
    }

    ret_val.diffuse_texture = cleanup_path(path.C_Str());

    return ret_val;
}

constexpr uint32_t FOURCC(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    return ((uint32_t) ((d << 24) | (c << 16) | (b << 8) | a));
}

void write_chunk(std::ofstream& out, uint32_t id, void* data, uint64_t size) {
    out.write(reinterpret_cast<char*>(&id), sizeof(id));
    out.write(reinterpret_cast<char*>(&size), sizeof(size));
    out.write(reinterpret_cast<char*>(data), (std::streamsize) size);
}

json_t* serializeMatrix(const aiMatrix4x4& mat) {
    json_t* mat_node = json_array();

    aiMatrix4x4 workMat = mat;
    workMat.Transpose(); // Row-major -> Column-major

    auto transform = glm::make_mat4x4(workMat[0]); // I hope this works...

    for (int i = 0; i < transform.length(); ++i) {
        auto column = transform[i];

        auto vec_array = json_array();

        for (int c = 0; c < column.length(); ++c) {
            json_array_append_new(vec_array, json_real(column[c]));
        }

        json_array_append_new(mat_node, vec_array);
    }

    return mat_node;
}

bool hasMesh(aiNode* node) {
    if (node->mNumMeshes > 0) {
        return true;
    }

    for (size_t i = 0; i < node->mNumChildren; ++i) {
        if (hasMesh(node->mChildren[i])) {
            return true;
        }
    }

    return false;
}
}

AssimpModelConverter::AssimpModelConverter() : _scene(nullptr) {
    createAILogger();

    // Indices are 16-bit, make sure that the "split large meshes" step enforces that
    _importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, 65536);

    // Colors are unused, remove them during import
    _importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_COLORS);

    // Remove degenerate faces
    _importer.SetPropertyInteger(AI_CONFIG_PP_FD_REMOVE, 1);
}

void AssimpModelConverter::convertModel(const std::string& input_file,
                                        const std::string& output_name,
                                        const std::string& output_directory) {
    _meshData.clear();
    _materials.clear();

    _materialMapping.clear();
    _meshMapping.clear();

    _scene = _importer.ReadFile(input_file.c_str(), DEFAULT_POST_PROCESSING_STEPS);

    std::ostringstream oss;
    oss << output_directory << "/" << output_name << ".fom";

    try {
        write_mesh_data(oss.str());
    } catch (const std::runtime_error& e) {
        fprintf(stderr, "Error while writing mesh data: %s\n", e.what());
        return;
    }

    oss.str("");
    oss << output_directory << "/" << output_name << ".json";
    try {
        auto json_root = serializeMetadata();
        FILE* f = std::fopen(oss.str().c_str(), "wb");
        json_dumpf(json_root, f, JSON_INDENT(4) | JSON_ENSURE_ASCII);
        json_decref(json_root);
        std::fclose(f);
    } catch (const std::runtime_error& e) {
        fprintf(stderr, "Error while writing meta data: %s\n", e.what());
        return;
    }
}

size_t AssimpModelConverter::getMaterialIndex(uint32_t aiIndex) {
    auto iter = _materialMapping.find(aiIndex);
    if (iter != _materialMapping.end()) {
        return iter->second;
    }

    auto converted = convertAssimpMaterial(_scene->mMaterials[aiIndex]);
    _materials.push_back(converted);

    size_t index = _materials.size() - 1;
    _materialMapping.insert(std::make_pair(aiIndex, index));

    return index;
}

void AssimpModelConverter::write_mesh_data(const std::string& output_file) {
    std::vector<ModelVertexData> vertex_data;
    std::vector<uint16_t> index_data;

    uint32_t index_offset = 0;
    size_t index_begin = 0;

    for (uint32_t i = 0; i < _scene->mNumMeshes; ++i) {
        auto mesh = _scene->mMeshes[i];
        if (mesh->mNumFaces == 0 || mesh->mNumVertices == 0) {
            throw std::runtime_error("A mesh was empty!");
        }
        if (mesh->mFaces[0].mNumIndices != 3) {
            continue;
        }
        if (!mesh->HasTextureCoords(0)) {
            throw std::runtime_error("Model needs to be textured!");
        }

        for (size_t vert = 0; vert < mesh->mNumVertices; ++vert) {
            auto& pos = mesh->mVertices[vert];
            auto& normal = mesh->mNormals[vert];
            auto& texCoord = mesh->mTextureCoords[0][vert];
            auto& tangent = mesh->mTangents[vert];
            auto& bitangent = mesh->mBitangents[vert];

            ModelVertexData data;
            data.position = glm::vec3(pos.x, pos.y, pos.z);
            data.normal = glm::vec3(normal.x, normal.y, normal.z);
            data.tex_coord = glm::vec3(texCoord.x, texCoord.y, 0.f);
            data.tangent = glm::vec3(tangent.x, tangent.y, tangent.z);
            data.bitangent = glm::vec3(bitangent.x, bitangent.y, bitangent.z);
            vertex_data.push_back(data);
        }

        std::pair<uint32_t, uint32_t> min_max_pair = std::make_pair(std::numeric_limits<uint32_t>::max(), 0);
        for (size_t index = 0; index < mesh->mNumFaces; ++index) {
            auto& face = mesh->mFaces[index];
            assert(face.mNumIndices == 3);
            index_data.push_back(process_index(face.mIndices[0], min_max_pair));
            index_data.push_back(process_index(face.mIndices[1], min_max_pair));
            index_data.push_back(process_index(face.mIndices[2], min_max_pair));
        }

        ExportMeshData data;
        data.name = mesh->mName.C_Str();
        data.material_index = getMaterialIndex(mesh->mMaterialIndex);

        data.offset = index_begin;
        data.count = mesh->mNumFaces * 3;

        data.base_index = index_offset;
        data.min_index = min_max_pair.first;
        data.max_index = min_max_pair.second;

        _meshData.push_back(data);
        _meshMapping.insert(std::make_pair(i, _meshData.size() - 1));

        index_offset += mesh->mNumVertices;
        index_begin += mesh->mNumFaces * 3;
    }

    std::ofstream outstream;
    outstream.open(output_file, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    if (!outstream.good()) {
        throw std::runtime_error("Failed to open output file!");
    }

    outstream.write("FSOMODEL", 8);
    uint32_t version = SUPPORTED_VERSION;
    outstream.write(reinterpret_cast<const char*>(&version), sizeof(version));

    // Write vertex data
    write_chunk(outstream, FOURCC('V', 'D', 'A', 'T'), vertex_data.data(), vertex_data.size() * sizeof(vertex_data[0]));

    // Write index data
    write_chunk(outstream, FOURCC('I', 'N', 'D', 'X'), index_data.data(), index_data.size() * sizeof(index_data[0]));

    outstream.flush();
    outstream.close();
}

json_t* AssimpModelConverter::serializeMetadata() {
    json_t* root = json_object();

    json_object_set_new(root, "materials", serializeMaterials());
    json_object_set_new(root, "meshes", serializeMeshes());
    json_object_set_new(root, "root_node", serializeNodeHierachy(_scene->mRootNode));

    return root;
}
json_t* AssimpModelConverter::serializeMaterials() {
    json_t* root = json_array();

    for (auto& mat : _materials) {
        auto* mat_obj = json_object();
        json_object_set_new(mat_obj, "name", json_string(mat.name.c_str()));
        json_object_set_new(mat_obj, "diffuse_texture", json_string(mat.diffuse_texture.c_str()));
        json_array_append_new(root, mat_obj);
    }

    return root;
}
json_t* AssimpModelConverter::serializeMeshes() {
    json_t* root = json_array();

    for (auto& mesh : _meshData) {
        auto* mesh_obj = json_object();

        json_object_set_new(mesh_obj, "name", json_string(mesh.name.c_str()));

        json_object_set_new(mesh_obj, "offset", json_integer((json_int_t) mesh.offset));
        json_object_set_new(mesh_obj, "count", json_integer((json_int_t) mesh.count));

        json_object_set_new(mesh_obj, "base_index", json_integer((json_int_t) mesh.base_index));
        json_object_set_new(mesh_obj, "min_index", json_integer((json_int_t) mesh.min_index));
        json_object_set_new(mesh_obj, "max_index", json_integer((json_int_t) mesh.max_index));

        json_object_set_new(mesh_obj, "material_index", json_integer((json_int_t) mesh.material_index));

        json_array_append_new(root, mesh_obj);
    }

    return root;
}
json_t* AssimpModelConverter::serializeNodeHierachy(aiNode* node) {
    auto root = json_object();

    json_object_set_new(root, "name", json_string(node->mName.C_Str()));
    json_object_set_new(root, "transform", serializeMatrix(node->mTransformation));

    auto mesh_array = json_array();
    for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
        auto index = node->mMeshes[i];
        auto iter = _meshMapping.find(index);
        if (iter == _meshMapping.end()) {
            throw std::runtime_error("Inconsistent data structure detected! Mesh mapping is not consistent!");
        }

        json_array_append_new(mesh_array, json_integer((json_int_t) iter->second));
    }
    json_object_set_new(root, "meshes", mesh_array);

    auto children_array = json_array();
    for (uint32_t i = 0; i < node->mNumChildren; ++i) {
        auto child = node->mChildren[i];

        if (hasMesh(child)) {
            // Only include nodes that have meshes or have children that have meshes
            json_array_append_new(children_array, serializeNodeHierachy(child));
        }
    }
    json_object_set_new(root, "children", children_array);

    return root;
}

