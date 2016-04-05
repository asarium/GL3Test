#pragma once

#include "renderer/BufferObject.hpp"

#include <glad/glad.h>

class GL3BufferObject : public BufferObject {
    GLuint _handle;
    BufferType _type;

public:
    GL3BufferObject(BufferType type);
    ~GL3BufferObject();

    void bind();

    virtual BufferType getType();

    virtual void setData(const void *data, size_t size, BufferUsage usage);

    virtual void updateData(const void *data, size_t size, size_t offset, UpdateFlags flags) override;
};


