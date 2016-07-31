//
//

#include "Model.hpp"

Model::Model(Renderer* renderer)
    : _renderer(renderer), _alignedUniformData(renderer->getLimits().uniform_offset_alignment), _numDrawCalls(0) {

    _vertexInputState.addComponent(AttributeType::Position,
                                   0,
                                   DataFormat::Vec3,
                                   offsetof(ModelVertexData, position));
    _vertexInputState.addComponent(AttributeType::TexCoord,
                                   0,
                                   DataFormat::Vec3,
                                   offsetof(ModelVertexData, tex_coord));
    _vertexInputState.addComponent(AttributeType::Normal,
                                   0,
                                   DataFormat::Vec3,
                                   offsetof(ModelVertexData, normal));
    _vertexInputState.addComponent(AttributeType::Tangent,
                                   0,
                                   DataFormat::Vec3,
                                   offsetof(ModelVertexData, tangent));
    _vertexInputState.addComponent(AttributeType::Bitangent,
                                   0,
                                   DataFormat::Vec3,
                                   offsetof(ModelVertexData, bitangent));

    _vertexInputState.addBufferBinding(0, false, sizeof(ModelVertexData));
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

    VertexArrayProperties vaoProps;
    vaoProps.addBufferBinding(0, _modelData.get());

    vaoProps.indexBuffer = _indexData.get();
    vaoProps.indexOffset = 0;
    vaoProps.indexType = IndexType::Short;

    _vertexArrayObject = _renderer->createVertexArrayObject(_vertexInputState, vaoProps);
}

void Model::setMeshData(std::vector<MeshData>&& data) {
    _meshData = std::move(data);
}

void Model::setMaterials(std::vector<Material>&& data) {
    _materials = std::move(data);
}
void Model::render(CommandBuffer* cmd) {
    cmd->bindVertexArrayObject(_vertexArrayObject.get());

    recursiveRender(cmd, _rootNode);
}
void Model::recursiveRender(CommandBuffer* cmd, const ModelNode& node) {
    for (auto& node_data : node.mesh_data) {
        auto& mesh = _meshData[node_data.mesh_index];

        cmd->bindDescriptorSet(node_data.model_descriptor_set.get());
        cmd->drawIndexed(mesh.vertex_count, 1, mesh.vertex_offset, mesh.base_vertex, 0);
    }

    for (auto& child : node.child_nodes) {
        recursiveRender(cmd, *child);
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
