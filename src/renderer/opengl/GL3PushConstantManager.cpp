//
//

#include "GL3PushConstantManager.hpp"
#include "GL3Renderer.hpp"
#include "EnumTranslation.hpp"

GL3PushConstantManager::GL3PushConstantManager(GL3Renderer* renderer) : GL3Object(renderer), _bufferSize(9) {
    _pushConstantBuffer.reset(static_cast<GL3BufferObject*>(renderer->createBuffer(BufferType::Uniform).release()));

    ensureBufferSize(128);
}

void GL3PushConstantManager::ensureBufferSize(size_t size) {
    if (_bufferSize >= size) {
        return;
    }

    _pushConstantBuffer->setData(nullptr, size, BufferUsage::Streaming);
}

void GL3PushConstantManager::setConstants(void* data, size_t size) {
    ensureBufferSize(size);

    _pushConstantBuffer->updateData(data, 0, size, UpdateFlags::DiscardOldData);

    glBindBufferRange(GL_UNIFORM_BUFFER,
                      mapDescriptorSetPartLocation(GL3DescriptorSetPart::PushConstantSet_Uniforms),
                      _pushConstantBuffer->getHandle(), 0, size);
}

