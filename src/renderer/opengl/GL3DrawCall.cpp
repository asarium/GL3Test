//
//

#include "GL3DrawCall.hpp"
#include "GL3DrawCallManager.hpp"

namespace {
    size_t getTypeSize(GLenum type) {
        switch (type) {
            case GL_UNSIGNED_SHORT:
                return sizeof(GLshort);
            case GL_UNSIGNED_INT:
                return sizeof(GLint);
            default:
                return sizeof(GLint);
        }
    }
}

GL3DrawCall::GL3DrawCall(const GL3DrawCallProperties &props) : _properties(props) {
}

GL3DrawCall::~GL3DrawCall() {
    // For OpenGL there is nothing to deallocate
}

void GL3DrawCall::setGLState() {
    _properties.vertexLayout->bind();
    _properties.state->setupState(&_parameters);
}

void GL3DrawCall::draw() {
    actualDraw(_properties.count, _properties.offset);
}

void GL3DrawCall::draw(size_t count, size_t offset) {
    actualDraw((GLsizei) count, (GLint) offset);
}

void GL3DrawCall::actualDraw(GLsizei count, GLint offset) {
    setGLState();

    if (_properties.indexed) {
        glDrawElements(_properties.primitive_type, count, _properties.index.type,
                       reinterpret_cast<void *>(offset * getTypeSize(_properties.index.type)));
    } else {
        glDrawArrays(_properties.primitive_type, offset, count);
    }
}

ShaderParameters *GL3DrawCall::getParameters() {
    return &_parameters;
}

