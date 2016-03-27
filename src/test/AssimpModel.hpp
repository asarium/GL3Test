#pragma once

#include <renderer/Renderer.hpp>
#include <assimp/scene.h>
#include "assimp/Importer.hpp"

class AssimpModel {
    Assimp::Importer _importer;
    const aiScene *_scene;

    std::unique_ptr<ShaderProgram> _shaderProgram;
    std::unique_ptr<Texture2D> _texture;

    std::vector<std::unique_ptr<DrawCall>> _sceneDrawCalls;

    std::unique_ptr<VertexLayout> _vertexLayout;

    std::unique_ptr<BufferObject> _vertexBuffer;
    std::unique_ptr<BufferObject> _indexBuffer;

    bool loadScene(const std::string &path);

    bool createVertexLayouts(Renderer *renderer);

    void recursiveRender(Renderer *renderer, const aiNode *node, const glm::mat4 &projection, const glm::mat4 &view,
                         const glm::mat4 &model);

public:
    AssimpModel();

    ~AssimpModel() { };

    bool loadModel(Renderer *renderer, const std::string &path);

    void drawModel(Renderer *renderer, const glm::mat4 &projection, const glm::mat4 &view, const glm::mat4 &model);
};


