//
//

#include "GL3BufferObject.hpp"

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
    glBindBuffer(getGLType(_type), _handle);
}

void GL3BufferObject::setData(void *data, size_t size, BufferUsage usage) {
    GLenum gl_type = getGLType(_type);
    glBindBuffer(gl_type, _handle);

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

    glBufferData(gl_type, size, data, gl_usage);

    glBindBuffer(gl_type, 0);
}