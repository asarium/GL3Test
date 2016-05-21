//
//

#include "VariableUniformBuffer.hpp"

VariableUniformBuffer::VariableUniformBuffer(Renderer* renderer) : _renderer(renderer), _size(0) {
    _uniformBuffer = _renderer->createBuffer(BufferType::Uniform);
}

void VariableUniformBuffer::setData(void* data, size_t size)
{
    if (size > _size)
    {
        _uniformBuffer->setData(data, size, BufferUsage::Streaming);
    } else
    {
        _uniformBuffer->updateData(data, 0, size, UpdateFlags::DiscardOldData);
    }
}

BufferObject* VariableUniformBuffer::buffer()
{
    return _uniformBuffer.get();
}

std::unique_ptr<VariableUniformBuffer> VariableUniformBuffer::createVariableBuffer(Renderer* renderer)
{
    return std::unique_ptr<VariableUniformBuffer>(new VariableUniformBuffer(renderer));
}
