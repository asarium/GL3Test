//
//

#include "Model.hpp"

Model::Model(Renderer* renderer) : _renderer(renderer) {

}

Model::~Model() {

}
void Model::setRootNode(ModelNode&& node) {
    _rootNode = std::move(node);
}

void Model::setModelData(std::unique_ptr<BufferObject>&& data_buffer, std::unique_ptr<BufferObject>&& index_buffer) {
    _modelData = std::move(data_buffer);
    _indexData = std::move(index_buffer);

    _vertexLayout = _renderer->createVertexLayout();
    auto modelIdx = _vertexLayout->attachBufferObject(_modelData.get());
    auto indexIdx = _vertexLayout->attachBufferObject(_indexData.get());

    _vertexLayout->setIndexBuffer(indexIdx);

    _vertexLayout->addComponent(AttributeType::Position,
                                DataFormat::Vec3,
                                sizeof(ModelVertexData),
                                modelIdx,
                                offsetof(ModelVertexData, position));
    _vertexLayout->addComponent(AttributeType::TexCoord,
                                DataFormat::Vec3,
                                sizeof(ModelVertexData),
                                modelIdx,
                                offsetof(ModelVertexData, tex_coord));
    _vertexLayout->addComponent(AttributeType::Normal,
                                DataFormat::Vec3,
                                sizeof(ModelVertexData),
                                modelIdx,
                                offsetof(ModelVertexData, normal));
    _vertexLayout->addComponent(AttributeType::Tangent,
                                DataFormat::Vec3,
                                sizeof(ModelVertexData),
                                modelIdx,
                                offsetof(ModelVertexData, tangent));
    _vertexLayout->addComponent(AttributeType::Bitangent,
                                DataFormat::Vec3,
                                sizeof(ModelVertexData),
                                modelIdx,
                                offsetof(ModelVertexData, bitangent));
    _vertexLayout->finalize();
}

void Model::setMeshData(std::vector<MeshData>&& data) {
    _meshData = std::move(data);
}

void Model::setMaterials(std::vector<Material>&& data) {
    _materials = std::move(data);
}
void Model::render(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model) {
    recursiveRender(_rootNode, projection, view, model);
}
void Model::recursiveRender(const ModelNode& node,
                            const glm::mat4& projection,
                            const glm::mat4& view,
                            const glm::mat4& model) {
    auto final_transform = model * node.transform;

    for (auto& mesh_idx : node.mesh_indices) {
        auto& mesh = _meshData[mesh_idx];

        auto params = mesh.mesh_draw_call->getParameters();
        params->setMat4(ShaderParameterType::ProjectionMatrix, projection);
        params->setMat4(ShaderParameterType::ViewMatrix, view);
        params->setMat4(ShaderParameterType::ModelMatrix, final_transform);

        mesh.mesh_draw_call->draw();
    }

    for (auto& child : node.child_nodes) {
        recursiveRender(*child, projection, view, final_transform);
    }
}

