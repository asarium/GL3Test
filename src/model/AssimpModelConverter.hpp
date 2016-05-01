#pragma once
//
//

#include "Model.hpp"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include <unordered_map>
#include <jansson.h>

struct ExportMeshData {
    std::string name;

    uint64_t offset;
    uint32_t count;

    uint32_t base_index;
    uint32_t min_index;
    uint32_t max_index;

    size_t material_index;
};

struct ExportMaterial {
    std::string name;

    std::string diffuse_texture;
};

class AssimpModelConverter {
    const aiScene* _scene;
    Assimp::Importer _importer;

    std::vector<ExportMeshData> _meshData;
    std::vector<ExportMaterial> _materials;

    std::unordered_map<uint32_t, size_t> _materialMapping; // assimp -> _materials
    std::unordered_map<uint32_t, size_t> _meshMapping; // assimp -> _meshData

    void write_mesh_data(const std::string& output_file);

    size_t getMaterialIndex(uint32_t aiIndex);

    json_t* serializeMetadata();
    json_t* serializeMaterials();
    json_t* serializeMeshes();
    json_t* serializeNodeHierachy(aiNode* node);
 public:
    AssimpModelConverter();

    void convertModel(const std::string& input_file,
                      const std::string& output_name, const std::string& output_directory);
};
