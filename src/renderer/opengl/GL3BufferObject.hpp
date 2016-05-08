#pragma once

#include "renderer/BufferObject.hpp"

#include <glad/glad.h>
#include <utility>

class GL3BufferObject final: public BufferObject {
    GLuint _handle;
    BufferType _type;
 public:
    explicit GL3BufferObject(BufferType type);
    ~GL3BufferObject();

    void bind();

    inline GLuint getHandle() {
        return _handle;
    }

    BufferType getType() const override;

    void setData(const void* data, size_t size, BufferUsage usage) override;

    void updateData(const void* data, size_t size, size_t offset, UpdateFlags flags) override;

    void* map(size_t size, size_t offset, BufferMapFlags flags) override;

    bool unmap() override;
};


