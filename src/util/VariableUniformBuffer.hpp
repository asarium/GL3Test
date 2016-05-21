#pragma once

#include <renderer/Renderer.hpp>

class VariableUniformBuffer {
    Renderer* _renderer;
    std::unique_ptr<BufferObject> _uniformBuffer;
    size_t _size;

 public:
    explicit VariableUniformBuffer(Renderer* renderer);

    void setData(void* data, size_t size);

    BufferObject* buffer();

    static std::unique_ptr<VariableUniformBuffer> createVariableBuffer(Renderer* renderer);
};


