//
//

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


BufferType GL3BufferObject::getType() {
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
    }
}

void GL3BufferObject::setData(void *data, size_t size, BufferUsage usage) {
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
    }
}
