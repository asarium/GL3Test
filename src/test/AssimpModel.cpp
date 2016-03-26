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

    for (size_t i = 0; i < _scene->mNumMeshes; ++i) {
        vertex_data.clear();
        indices.clear();

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
            indices.push_back(face.mIndices[0]);
            indices.push_back(face.mIndices[1]);
            indices.push_back(face.mIndices[2]);
        }

        auto vertex_buffer = renderer->createBuffer(BufferType::Vertex);
        vertex_buffer->setData(vertex_data.data(), vertex_data.size() * sizeof(vertex_data[0]), BufferUsage::Static);

        auto index_buffer = renderer->createBuffer(BufferType::Index);
        index_buffer->setData(indices.data(), indices.size() * sizeof(indices[0]), BufferUsage::Static);

        auto layout = renderer->createVertexLayout();
        auto vertex_idx = layout->attachBufferObject(vertex_buffer.get());
        auto index_idx = layout->attachBufferObject(index_buffer.get());

        layout->addComponent(AttributeType::Position, DataFormat::Vec3, sizeof(VertexData), vertex_idx,
                             offsetof(VertexData, position));
        layout->addComponent(AttributeType::Normal, DataFormat::Vec3, sizeof(VertexData), vertex_idx,
                             offsetof(VertexData, normal));
        layout->addComponent(AttributeType::TexCoord, DataFormat::Vec2, sizeof(VertexData), vertex_idx,
                             offsetof(VertexData, tex_coord));

        layout->setIndexBuffer(index_idx);

        layout->finalize();

        DrawCallProperties props;
        props.shader = _shaderProgram.get();
        props.vertexLayout = layout.get();
        props.state.depth_test = true;

        auto drawCall = renderer->getDrawCallManager()->createIndexedCall(props, PrimitiveType::Triangle,
                                                                          0, indices.size(), IndexType::Integer);

        _bufferObects.push_back(std::move(vertex_buffer));
        _bufferObects.push_back(std::move(index_buffer));
        _vertexLayouts.push_back(std::move(layout));
        _sceneDrawCalls.push_back(std::move(drawCall));
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


