//
//

#include <cstring>
#include "GL3BufferObject.hpp"
#include "GL3State.hpp"

namespace {
    GLenum getGLType(BufferType type) {
        switch (type) {
            case BufferType::None:
                return GL_INVALID_ENUM;
            case BufferType::Vertex:
                return GL_ARRAY_BUFFER;
            case BufferType::Index:
                return GL_ELEMENT_ARRAY_BUFFER;
            case BufferType::Uniform:
                return GL_UNIFORM_BUFFER;
        }
        return GL_INVALID_ENUM;
    }
}

GL3BufferObject::GL3BufferObject(BufferType type) {
    glGenBuffers(1, &_handle);
    _type = type;
}

GL3BufferObject::~GL3BufferObject() {
    glDeleteBuffers(1, &_handle);
}


BufferType GL3BufferObject::getType() const {
    return _type;
}

void GL3BufferObject::bind() {
    switch(_type) {
        case BufferType::None:
            return;
        case BufferType::Vertex:
            GLState->Buffer.bindArrayBuffer(_handle);
            break;
        case BufferType::Index:
            GLState->Buffer.bindElementBuffer(_handle);
            break;
        case BufferType::Uniform:
            GLState->Buffer.bindUniformBuffer(_handle);
            break;
    }
}

void GL3BufferObject::setData(const void *data, size_t size, BufferUsage usage) {
    this->bind();

    GLenum gl_usage;
    switch (usage) {
        case BufferUsage::Static:
            gl_usage = GL_STATIC_DRAW;
            break;
        case BufferUsage::Dynamic:
            gl_usage = GL_DYNAMIC_DRAW;
            break;
        case BufferUsage::Streaming:
            gl_usage = GL_STREAM_DRAW;
            break;
    }

    GLenum gl_type = getGLType(_type);
    glBufferData(gl_type, size, data, gl_usage);

    switch(_type) {
        case BufferType::None:
            return;
        case BufferType::Vertex:
            GLState->Buffer.bindArrayBuffer(0);
            break;
        case BufferType::Index:
            GLState->Buffer.bindElementBuffer(0);
            break;
        case BufferType::Uniform:
            GLState->Buffer.bindUniformBuffer(0);
            break;
    }
}

void GL3BufferObject::updateData(const void *data, size_t offset, size_t size, UpdateFlags flags) {
    this->bind();
    if (flags & UpdateFlags::DiscardOldData) {
        auto ptr = map(offset, size, BufferMapFlags::Write | BufferMapFlags::InvalidateData);

        std::memcpy(ptr, data, size);

        if (unmap()) {
            // Unmap successful
            return;
        }
        // If we are still here then the unmap was not successful, try using glBufferSubData
    }
    // Just use bufferSubData
    glBufferSubData(getGLType(_type), offset, size, data);
}

void* GL3BufferObject::map(size_t offset, size_t size, BufferMapFlags flags)
{
    Assertion(size > 0, "Size may not be zero!");

    GLbitfield gl_flags = 0;
    if (flags & BufferMapFlags::Read)
    {
        gl_flags |= GL_MAP_READ_BIT;
    }
    if (flags & BufferMapFlags::Write)
    {
        gl_flags |= GL_MAP_WRITE_BIT;
    }
    if (flags & BufferMapFlags::InvalidateData)
    {
        gl_flags |= GL_MAP_INVALIDATE_RANGE_BIT;
    }

    this->bind();
    return glMapBufferRange(getGLType(_type), offset, size, gl_flags);
}

bool GL3BufferObject::unmap()
{
    this->bind();
    return glUnmapBuffer(getGLType(_type)) == GL_TRUE;
}
