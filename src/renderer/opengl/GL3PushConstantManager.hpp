#pragma once

#include "GL3Object.hpp"
#include "GL3BufferObject.hpp"
#include <renderer/BufferObject.hpp>

#include <memory>

class GL3PushConstantManager : public GL3Object {
    std::unique_ptr<GL3BufferObject> _pushConstantBuffer;
    size_t _bufferSize;

    void ensureBufferSize(size_t size);
 public:
    GL3PushConstantManager(GL3Renderer* renderer);

    void setConstants(void* data, size_t size);
};


