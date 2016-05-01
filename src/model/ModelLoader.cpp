//
//

#include "ModelLoader.hpp"

#include <sstream>
#include <util/textures.hpp>
#include <fstream>
#include <cstring>

namespace {
const int SUPPORTED_VERSION = 1;

constexpr uint32_t FOURCC(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    return ((uint32_t) ((d << 24) | (c << 16) | (b << 8) | a));
}

glm::vec4 parseVector(json_t* vector_node) {
    glm::vec4 out;

    if (json_array_size(vector_node) != 4) {
        throw std::runtime_error("Invalid number of values in vector array!");
    }

    size_t index;
    json_t* value;
    json_array_foreach(vector_node, index, value) {
        out[index] = (float) json_real_value(value);
    }

    return out;
}

glm::mat4 parseMatrix(json_t* matrix_node) {
    glm::mat4 mat;

    if (json_array_size(matrix_node) != 4) {
        throw std::runtime_error("Invalid number of values in matrix array!");
    }

    size_t index;
    json_t* value;
    json_array_foreach(matrix_node, index, value) {
        mat[index] = parseVector(value);
    }

    return mat;
}
}

ModelLoader::ModelLoader(Renderer* renderer) : _renderer(renderer) {

}
std::unique_ptr<Model> ModelLoader::loadModel(const std::string& model_name) {
    std::unique_ptr<Model> out_model(new Model(_renderer));

    std::stringstream name_stream;
    name_stream << model_name << ".fom";
    if (!loadModelData(out_model.get(), name_stream.str())) {
        return nullptr;
    }

//    auto model_data = _loader->getFileContents(name_stream.str());

    name_stream.str("");
    name_stream << model_name << ".json";
    json_error_t err;
    json_t* metadata = json_load_file(name_stream.str().c_str(), 0, &err);
    if (!metadata) {
        fprintf(stderr, "[%s (%d:%d)] %s\n", err.source, err.line, err.column, err.text);
        return nullptr;
    }

    if (!loadMetaData(out_model.get(), metadata)) {
        json_decref(metadata);
        return nullptr;
    }
    json_decref(metadata);

    return out_model;
}

bool ModelLoader::loadModelData(Model* output, const std::string& file_path) {
    std::ifstream model_data_stream;
    model_data_stream.open(file_path, std::ios_base::in | std::ios_base::binary);
    if (!model_data_stream.good()) {
        fprintf(stderr, "Failed to open model file!\n");
        return false;
    }

    char header_id[8];
    model_data_stream.read(header_id, sizeof(header_id));
    if (!model_data_stream.good()) {
        fprintf(stderr, "Failed to read header of model data!\n");
        return false;
    }
    if (strcmp(header_id, "FSOMODEL")) {
        fprintf(stderr, "Header of model is not valid!\n");
        return false;
    }

    uint32_t version;
    model_data_stream.read(reinterpret_cast<char*>(&version), sizeof(version));
    if (!model_data_stream.good()) {
        fprintf(stderr, "Failed to read header version of model data!\n");
        return false;
    }
    if (version != SUPPORTED_VERSION) {
        fprintf(stderr, "Version of model file is not supported!\n");
        return false;
    }

    std::unique_ptr<BufferObject> vertexObject = _renderer->createBuffer(BufferType::Vertex);
    std::unique_ptr<BufferObject> indexObject = _renderer->createBuffer(BufferType::Index);

    bool vertexDataRead = false;
    bool indexDataRead = false;

    while (true) {
        uint32_t chunk_type;
        model_data_stream.read(reinterpret_cast<char*>(&chunk_type), sizeof(chunk_type));
        if (!model_data_stream.good()) {
            if (model_data_stream.eof()) {
                // Expected EOF encountered
                break;
            }
            fprintf(stderr, "Failed to read chunk_type identifier!\n");
            return false;
        }
        uint64_t chunk_length;
        model_data_stream.read(reinterpret_cast<char*>(&chunk_length), sizeof(chunk_length));
        if (!model_data_stream.good()) {
            fprintf(stderr, "Failed to read chunk_type length!\n");
            return false;
        }

        switch (chunk_type) {
            case FOURCC('V', 'D', 'A', 'T'): {
                if (vertexDataRead) {
                    fprintf(stderr, "Encountered duplicate vertex data chunk!!\n");
                    return false;
                }

                std::vector<uint8_t> vertex_data;
                vertex_data.resize((size_t) chunk_length);
                model_data_stream.read(reinterpret_cast<char*>(vertex_data.data()), vertex_data.size());
                if (!model_data_stream.good()) {
                    fprintf(stderr, "Failed to read vertex data!\n");
                    return false;
                }

                vertexObject->setData(vertex_data.data(), (size_t) chunk_length, BufferUsage::Static);
                vertexDataRead = true;

                break;
            }
            case FOURCC('I', 'N', 'D', 'X'): {
                if (indexDataRead) {
                    fprintf(stderr, "Encountered duplicate index data chunk!!\n");
                    return false;
                }

                std::vector<uint8_t> index_data;
                index_data.resize((size_t) chunk_length);
                model_data_stream.read(reinterpret_cast<char*>(index_data.data()), index_data.size());
                if (!model_data_stream.good()) {
                    fprintf(stderr, "Failed to read vertex data!\n");
                    return false;
                }

                indexObject->setData(index_data.data(), (size_t) chunk_length, BufferUsage::Static);
                indexDataRead = true;

                break;
            }
            default:
                fprintf(stderr, "Skipping unknown chunk_type type %x.\n", chunk_type);
                model_data_stream.seekg(chunk_length, std::ios_base::cur);
                break;
        }
    }
    output->setModelData(std::move(vertexObject), std::move(indexObject));

    return true;
}

bool ModelLoader::loadMetaData(Model* output, json_t* metadata) {
    auto materials = json_object_get(metadata, "materials");
    if (!materials) {
        fprintf(stderr, "Materials key could not be found!\n");
        return false;
    }
    if (!loadMaterials(output, materials)) {
        return false;
    }

    auto meshes = json_object_get(metadata, "meshes");
    if (!meshes) {
        fprintf(stderr, "Meshes key could not be found!\n");
        return false;
    }
    if (!loadMeshes(output, meshes)) {
        return false;
    }

    auto root_node = json_object_get(metadata, "root_node");
    if (!root_node) {
        fprintf(stderr, "Root node value is missing!\n");
    }
    if (!loadNodes(output, root_node)) {
        return false;
    }

    return true;
}
bool ModelLoader::loadMaterials(Model* output, json_t* materials_root) {
    std::vector<Material> materials;

    size_t index;
    json_t* value;
    json_array_foreach(materials_root, index, value) {
        auto name_node = json_object_get(value, "name");
        auto diffuse_node = json_object_get(value, "diffuse_texture");

        if (!diffuse_node) {
            fprintf(stderr, "Malformed materials entry encountered!\n");
            return false;
        }

        Material mat;
        mat.name = name_node == nullptr ? "" : json_string_value(name_node);
        mat.diffuse_texture = util::load_texture(_renderer, std::string("resources/") + json_string_value(diffuse_node));

        materials.push_back(std::move(mat));
    }

    output->setMaterials(std::move(materials));
    return true;
}
bool ModelLoader::loadMeshes(Model* output, json_t* meshes_root) {
    std::vector<MeshData> meshData;

    auto& materials = output->getMaterials();

    size_t index;
    json_t* value;
    json_array_foreach(meshes_root, index, value) {
        auto name_node = json_object_get(value, "name");

        auto offset_node = json_object_get(value, "offset");
        auto count_node = json_object_get(value, "count");

        auto base_index_node = json_object_get(value, "base_index");
        auto min_index_node = json_object_get(value, "min_index");
        auto max_index_node = json_object_get(value, "max_index");

        auto material_index_node = json_object_get(value, "material_index");

        if (!offset_node) {
            fprintf(stderr, "Offset node is not present!!\n");
            return false;
        }
        if (!count_node) {
            fprintf(stderr, "Count node is not present!!\n");
            return false;
        }

        if (!base_index_node) {
            fprintf(stderr, "Base index node is not present!!\n");
            return false;
        }
        if (!min_index_node) {
            fprintf(stderr, "Min index node is not present!!\n");
            return false;
        }
        if (!max_index_node) {
            fprintf(stderr, "Max index node is not present!!\n");
            return false;
        }

        if (!material_index_node) {
            fprintf(stderr, "Material index node is not present!!\n");
            return false;
        }

        auto& material = materials[json_integer_value(material_index_node)];

        MeshData mesh;
        mesh.mesh_name = name_node == nullptr ? "" : json_string_value(name_node);
        mesh.material_index = (size_t) json_integer_value(material_index_node);

        DrawCallCreateProperties props;
        props.vertexLayout = output->getVertexLayout().get();

        props.primitive_type = PrimitiveType::Triangle;

        props.offset = (size_t) json_integer_value(offset_node);
        props.count = (size_t) json_integer_value(count_node);

        props.index_type = IndexType::Short;

        props.base_vertex = (uint32_t) json_integer_value(base_index_node);

        props.min_index = (uint32_t) json_integer_value(min_index_node);
        props.max_index = (uint32_t) json_integer_value(max_index_node);

        mesh.mesh_draw_call = _renderer->getDrawCallManager()->createDrawCall(props);

        mesh.mesh_draw_call->getParameters()->setTexture(ShaderParameterType::ColorTexture,
                                                         material.diffuse_texture.get());

        meshData.push_back(std::move(mesh));
    }

    output->setMeshData(std::move(meshData));
    return true;
}

bool ModelLoader::loadNodes(Model* output, json_t* nodes_root) {
    ModelNode rootNode;
    if (!parseModelNode(nodes_root, rootNode)) {
        return false;
    }

    output->setRootNode(std::move(rootNode));
    return true;
}
bool ModelLoader::parseModelNode(json_t* json_node, ModelNode& node_out) {
    auto name_node = json_object_get(json_node, "name");
    auto transform_node = json_object_get(json_node, "transform");

    auto meshes_node = json_object_get(json_node, "meshes");
    auto children_node = json_object_get(json_node, "children");

    if (!transform_node) {
        fprintf(stderr, "Transform node is missing!\n");
        return false;
    }

    if (!meshes_node) {
        fprintf(stderr, "Meshes node is missing!\n");
        return false;
    }
    if (!children_node) {
        fprintf(stderr, "Children node is missing!\n");
        return false;
    }

    node_out.name = name_node == nullptr ? "" : json_string_value(name_node);
    try {
        node_out.transform = parseMatrix(transform_node);
    } catch(const std::runtime_error& e) {
        fprintf(stderr, "Failed to parse transform matrix: %s\n", e.what());
        return false;
    }

    size_t index;
    json_t* value;
    json_array_foreach(meshes_node, index, value) {
        node_out.mesh_indices.push_back((size_t) json_integer_value(value));
    }

    json_array_foreach(children_node, index, value) {
        std::unique_ptr<ModelNode> child_node(new ModelNode());
        if (!parseModelNode(value, *child_node)) {
            return false;
        }
        node_out.child_nodes.push_back(std::move(child_node));
    }

    return true;
}

