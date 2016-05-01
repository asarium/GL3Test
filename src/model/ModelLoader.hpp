#pragma once

#include <renderer/Renderer.hpp>
#include <jansson.h>
#include "Model.hpp"

class ModelLoader {
    Renderer* _renderer;

    bool loadMetaData(Model* output, json_t* metadata);

    bool loadMaterials(Model* output, json_t* materials_root);

    bool loadMeshes(Model* output, json_t* meshes_root);

    bool loadNodes(Model* output, json_t* nodes_root);

    bool parseModelNode(json_t* json_node, ModelNode & node_out);

    bool loadModelData(Model* output, const std::string& file_path);
 public:
    ModelLoader(Renderer* renderer);

    std::unique_ptr<Model> loadModel(const std::string& model_name);
};
