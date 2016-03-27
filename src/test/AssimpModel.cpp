//
//

#include "AssimpModel.hpp"

#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/scene.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include <glm/gtx/string_cast.hpp>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

namespace {
    struct VertexData {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 tex_coord;
    };

    bool loggerCreated = false;

    void createAILogger() {
        if (loggerCreated) {
            return;
        }
        loggerCreated = true;

        // Change this line to normal if you not want to analyse the import process
        //Assimp::Logger::LogSeverity severity = Assimp::Logger::NORMAL;
        Assimp::Logger::LogSeverity severity = Assimp::Logger::VERBOSE;

        // Create a logger instance for Console Output
        Assimp::DefaultLogger::create("", severity, aiDefaultLogStream_STDOUT);

        // Now I am ready for logging my stuff
        Assimp::DefaultLogger::get()->info("this is my info-call");
    }

    void logInfo(std::string logString) {
        // Will add message to File with "info" Tag
        Assimp::DefaultLogger::get()->info(logString.c_str());
    }

    void logDebug(const char *logString) {
        // Will add message to File with "debug" Tag
        Assimp::DefaultLogger::get()->debug(logString);
    }
}

AssimpModel::AssimpModel() : _scene(nullptr) {
    createAILogger();
}

bool AssimpModel::loadModel(Renderer *renderer, const std::string &path) {
    if (!loadScene(path)) {
        return false;
    }

    _texture = renderer->createTexture();
    int x, y, n;
    auto texture_data = stbi_load("resources/logo-full.png", &x, &y, &n, 0);
    if (texture_data) {
        _texture->initialize(x, y, n == 3 ? TextureFormat::R8G8B8 : TextureFormat::R8G8B8A8, texture_data);

        stbi_image_free(texture_data);
    }

    if (!createVertexLayouts(renderer)) {
        return false;
    }

    return true;
}

bool AssimpModel::loadScene(const std::string &path) {
    _scene = _importer.ReadFile(path, aiProcessPreset_TargetRealtime_Quality);
    // If the import failed, report it
    if (!_scene) {
        logInfo(_importer.GetErrorString());
        return false;
    }

    return true;
}

bool AssimpModel::createVertexLayouts(Renderer *renderer) {
    _shaderProgram = renderer->createShader(ShaderType::Model);

    std::vector<VertexData> vertex_data;
    std::vector<uint32_t> indices;
    uint32_t meshOffset = 0;

    for (size_t i = 0; i < _scene->mNumMeshes; ++i) {
        auto mesh = _scene->mMeshes[i];

        for (size_t vert = 0; vert < mesh->mNumVertices; ++vert) {
            const aiVector3D &pPos = mesh->mVertices[vert];
            const aiVector3D &pNormal = mesh->HasNormals() ? mesh->mNormals[vert] : aiVector3D();
            const aiVector3D &pTexCoord = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][vert] : aiVector3D();

            VertexData data;
            data.position = glm::vec3(pPos.x, pPos.y, pPos.z);
            data.normal = glm::vec3(pNormal.x, pNormal.y, pNormal.z);
            data.tex_coord = glm::vec2(pTexCoord.x, pTexCoord.y);
            vertex_data.push_back(data);
        }

        for (size_t index = 0; index < mesh->mNumFaces; ++index) {
            auto &face = mesh->mFaces[index];
            assert(face.mNumIndices == 3);
            indices.push_back(face.mIndices[0] + meshOffset);
            indices.push_back(face.mIndices[1] + meshOffset);
            indices.push_back(face.mIndices[2] + meshOffset);
        }
        // We need to keep track of where the current mesh begins to make sure that the indices still match
        meshOffset += mesh->mNumVertices;
    }

    _vertexBufferObect = renderer->createBuffer(BufferType::Vertex);
    _vertexBufferObect->setData(vertex_data.data(), vertex_data.size() * sizeof(vertex_data[0]), BufferUsage::Static);

    _indexBufferObect = renderer->createBuffer(BufferType::Index);
    _indexBufferObect->setData(indices.data(), indices.size() * sizeof(indices[0]), BufferUsage::Static);

    _vertexLayout = renderer->createVertexLayout();
    auto vertex_idx = _vertexLayout->attachBufferObject(_vertexBufferObect.get());
    auto index_idx = _vertexLayout->attachBufferObject(_indexBufferObect.get());

    _vertexLayout->addComponent(AttributeType::Position, DataFormat::Vec3, sizeof(VertexData), vertex_idx,
                                offsetof(VertexData, position));
    _vertexLayout->addComponent(AttributeType::Normal, DataFormat::Vec3, sizeof(VertexData), vertex_idx,
                                offsetof(VertexData, normal));
    _vertexLayout->addComponent(AttributeType::TexCoord, DataFormat::Vec2, sizeof(VertexData), vertex_idx,
                                offsetof(VertexData, tex_coord));

    _vertexLayout->setIndexBuffer(index_idx);

    _vertexLayout->finalize();

    meshOffset = 0;
    for (size_t i = 0; i < _scene->mNumMeshes; ++i) {
        auto mesh = _scene->mMeshes[i];

        DrawCallProperties props;
        props.shader = _shaderProgram.get();
        props.vertexLayout = _vertexLayout.get();
        props.state.depth_test = true;

        auto drawCall = renderer->getDrawCallManager()->createIndexedCall(props, PrimitiveType::Triangle,
                                                                          meshOffset, mesh->mNumFaces * 3,
                                                                          IndexType::Integer);
        drawCall->getParameters()->setTexture(ShaderParameterType::ColorTexture, _texture.get());

        _sceneDrawCalls.push_back(std::move(drawCall));
        // We need to keep track of where the current mesh begins to make sure that the indices still match
        meshOffset += mesh->mNumFaces * 3;
    }

    return true;
}

void AssimpModel::drawModel(Renderer *renderer, const glm::mat4 &projection, const glm::mat4 &view,
                            const glm::mat4 &model) {
    recursiveRender(renderer, _scene->mRootNode, projection, view, model);
}

void AssimpModel::recursiveRender(Renderer *renderer, const aiNode *node, const glm::mat4 &projection,
                                  const glm::mat4 &view, const glm::mat4 &model) {
    auto final_transform = model;
    if (node->mParent) {
        auto aiTransform = node->mTransformation;
        aiTransform.Transpose(); // Row major -> column major
        auto transform = glm::make_mat4x4(&aiTransform.a1); // I hope this works...

        final_transform = model * transform;
    }

    for (size_t i = 0; i < node->mNumMeshes; ++i) {
        auto &drawCall = _sceneDrawCalls[node->mMeshes[i]];

        auto params = drawCall->getParameters();
        params->setMat4(ShaderParameterType::ProjectionMatrix, projection);
        params->setMat4(ShaderParameterType::ViewMatrix, view);
        params->setMat4(ShaderParameterType::ModelMatrix, final_transform);

        drawCall->draw();
    }

    for (size_t i = 0; i < node->mNumChildren; ++i) {
        recursiveRender(renderer, node->mChildren[i], projection, view, final_transform);
    }
}


