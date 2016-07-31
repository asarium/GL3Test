#pragma once

#include <renderer/BufferObject.hpp>
#include <renderer/VertexLayout.hpp>
#include <renderer/Renderer.hpp>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include <memory>
#include <vector>
#include <util/UniformAligner.hpp>

struct ModelVertexData {
    glm::vec3 position;
    glm::vec3 tex_coord;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

struct Material {
    std::string name;
    std::unique_ptr<Texture> diffuse_texture;
};

struct MeshData {
    std::string mesh_name;

    size_t material_index;

    uint32_t vertex_offset;
    uint32_t vertex_count;

    uint32_t base_vertex;

    MeshData() : material_index(0) { }
};

struct NodeMeshData {
    size_t mesh_index;
    std::unique_ptr<DescriptorSet> model_descriptor_set;
};

struct ModelNode {
    std::string name;
    glm::mat4 transform;

    size_t index;

    std::vector<NodeMeshData> mesh_data;
    std::vector<std::unique_ptr<ModelNode>> child_nodes;

    ModelNode() : index(0) { }
};

class Model {
    std::unique_ptr<BufferObject> _modelData;
    std::unique_ptr<BufferObject> _indexData;
    std::unique_ptr<BufferObject> _nodeUniformData;

    std::unique_ptr<VertexArrayObject> _vertexArrayObject;

    Renderer* _renderer;

    UniformAligner<ModelUniformData> _alignedUniformData;

    std::vector<MeshData> _meshData;
    std::vector<Material> _materials;

    ModelNode _rootNode;

    size_t _numDrawCalls;

    VertexInputStateProperties _vertexInputState;

    size_t updateNodeIndices(ModelNode& node, size_t nextIndex);

    void initializeDescriptorSets(ModelNode& node);

    void updateUniformData(const ModelNode& node, const glm::mat4& model);

    void recursiveRender(CommandBuffer* cmd, const ModelNode& node);
 public:
    explicit Model(Renderer* renderer);
    ~Model();

    void setRootNode(ModelNode&& node);

    void setMeshData(std::vector<MeshData>&& data);

    void setMaterials(std::vector<Material>&& data);

    void setModelData(std::unique_ptr<BufferObject>&& data_buffer, std::unique_ptr<BufferObject>&& index_buffer);

    void prepareData(const glm::mat4& world_transform);

    void render(CommandBuffer* cmd);

    const std::vector<MeshData>& getMeshData() const {
        return _meshData;
    }
    const std::vector<Material>& getMaterials() const {
        return _materials;
    }

    const VertexInputStateProperties& getVertexInputState() const {
        return _vertexInputState;
    }
};
