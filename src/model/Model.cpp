//
//

#include "Model.hpp"

Model::Model(Renderer* renderer)
    : _renderer(renderer), _alignedUniformData(renderer->getLimits().uniform_offset_alignment), _numDrawCalls(0) {

}

Model::~Model() {

}
void Model::setRootNode(ModelNode&& node) {
    _rootNode = std::move(node);

    _numDrawCalls = updateNodeIndices(_rootNode, 0);

    _alignedUniformData.resize(_numDrawCalls);

    _nodeUniformData = _renderer->createBuffer(BufferType::Uniform);
    _nodeUniformData->setData(nullptr, _alignedUniformData.getSize(), BufferUsage::Streaming);

    initializeDescriptorSets(_rootNode);
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
void Model::render() {
    recursiveRender(_rootNode);
}
void Model::recursiveRender(const ModelNode& node) {
    for (auto& node_data : node.mesh_data) {
        auto& mesh = _meshData[node_data.mesh_index];

        node_data.model_descriptor_set->bind();
        mesh.mesh_draw_call->draw();
        node_data.model_descriptor_set->unbind();
    }

    for (auto& child : node.child_nodes) {
        recursiveRender(*child);
    }
}

void Model::prepareData(const glm::mat4& world_transform) {
    updateUniformData(_rootNode, world_transform);

    _nodeUniformData->updateData(_alignedUniformData.getData(),
                                 0,
                                 _alignedUniformData.getSize(),
                                 UpdateFlags::DiscardOldData);
}
size_t Model::updateNodeIndices(ModelNode& node, size_t nextIndex) {
    node.index = nextIndex;
    nextIndex += node.mesh_data.size();

    for (auto& child : node.child_nodes) {
        nextIndex = updateNodeIndices(*child, nextIndex);
    }

    return nextIndex;
}
void Model::initializeDescriptorSets(ModelNode& node) {
    size_t i = 0;
    for (auto& node_data : node.mesh_data) {
        auto& mesh = _meshData[node_data.mesh_index];
        auto& material = _materials[mesh.material_index];

        auto descriptor_set = _renderer->createDescriptorSet(DescriptorSetType::ModelSet);
        descriptor_set->getDescriptor(DescriptorSetPart::ModelSet_DiffuseTexture)->setTexture(material.diffuse_texture.get());
        descriptor_set->getDescriptor(DescriptorSetPart::ModelSet_Uniforms)->setUniformBuffer(_nodeUniformData.get(),
                                                                                              _alignedUniformData.getOffset(
                                                                                                  node.index + i),
                                                                                              sizeof(ModelUniformData));

        node_data.model_descriptor_set = std::move(descriptor_set);

        ++i;
    }

    for (auto& child : node.child_nodes) {
        initializeDescriptorSets(*child);
    }
}
void Model::updateUniformData(const ModelNode& node, const glm::mat4& model) {
    auto final_transform = model * node.transform;

    auto data = _alignedUniformData.getElement(node.index);
    data->model_matrix = final_transform;
    data->normal_model_matrix = glm::transpose(glm::inverse(final_transform));

    for (auto& child : node.child_nodes) {
        updateUniformData(*child, final_transform);
    }
}
