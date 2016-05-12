#pragma once

#include <renderer/Renderer.hpp>
#include <jansson.h>
#include "Model.hpp"

class ModelLoader {
    Renderer* _renderer;

    std::unique_ptr<Model> _currentModel;

    bool loadMetaData(json_t* metadata);

    bool loadMaterials(json_t* materials_root);

    bool loadMeshes(json_t* meshes_root);

    bool loadNodes(json_t* nodes_root);

    bool parseModelNode(json_t* json_node, ModelNode & node_out);

    bool loadModelData(const std::string& file_path);
 public:
    ModelLoader(Renderer* renderer);

    std::unique_ptr<Model> loadModel(const std::string& model_name);
};
