#pragma once

#include <renderer/BufferObject.hpp>
#include <renderer/VertexLayout.hpp>
#include <renderer/Renderer.hpp>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include <memory>
#include <vector>

struct ModelVertexData {
    glm::vec3 position;
    glm::vec3 tex_coord;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

struct ModelNode {
    std::string name;
    glm::mat4 transform;

    std::vector<size_t> mesh_indices;
    std::vector<std::unique_ptr<ModelNode>> child_nodes;
};

struct Material {
    std::string name;
    std::unique_ptr<Texture2D> diffuse_texture;
};

struct MeshData {
    std::string mesh_name;

    size_t material_index;

    std::unique_ptr<DrawCall> mesh_draw_call;
};

class Model {
    std::unique_ptr<BufferObject> _modelData;
    std::unique_ptr<BufferObject> _indexData;

    std::unique_ptr<VertexLayout> _vertexLayout;

    std::vector<MeshData> _meshData;
    std::vector<Material> _materials;

    ModelNode _rootNode;

    Renderer* _renderer;

    void recursiveRender(const ModelNode& node, const glm::mat4& projection, const glm::mat4& view,
                         const glm::mat4& model);
 public:
    Model(Renderer* renderer);
    ~Model();

    void setRootNode(ModelNode&& node);

    void setMeshData(std::vector<MeshData>&& data);

    void setMaterials(std::vector<Material>&& data);

    void setModelData(std::unique_ptr<BufferObject>&& data_buffer, std::unique_ptr<BufferObject>&& index_buffer);

    void render(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model);

    const std::vector<MeshData>& getMeshData() const {
        return _meshData;
    }
    const std::vector<Material>& getMaterials() const {
        return _materials;
    }

    const std::unique_ptr<VertexLayout>& getVertexLayout() const {
        return _vertexLayout;
    }
};