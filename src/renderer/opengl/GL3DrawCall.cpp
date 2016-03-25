//
//

#include "GL3DrawCall.hpp"

namespace {
    GLenum getPrimitiveType(PrimitiveType type) {
        switch(type) {
            case PrimitiveType::Triangle:
                return GL_TRIANGLES;
        }
    }

    GLenum getIndexType(IndexType type) {
        switch(type) {
            case IndexType::Short:
                return GL_UNSIGNED_SHORT;
            case IndexType::Integer:
                return GL_UNSIGNED_INT;
        }
    }
}

GL3DrawCall::GL3DrawCall(const GL3DrawCallProperties &props) : _properties(props) {
}

GL3DrawCall::~GL3DrawCall() {
    // For OpenGL there is nothing to deallocate
}

void GL3DrawCall::setGLState() {
    _properties.shader->bind();
    _properties.vertexLayout->bind();

    if (_properties.state.depth_test) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

void GL3DrawCall::draw(PrimitiveType type, size_t count) {
    setGLState();

    glDrawArrays(getPrimitiveType(type), 0, static_cast<GLsizei>(count));
}

void GL3DrawCall::drawIndexed(PrimitiveType type, size_t count, IndexType indexType) {
    setGLState();

    glDrawElements(getPrimitiveType(type), static_cast<GLsizei>(count), getIndexType(indexType), nullptr);
}
