//
//

#include "GL3DrawCall.hpp"

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

void GL3DrawCall::draw() {
    setGLState();

    if (_properties.indexed) {
        glDrawElements(_properties.primitive_type, _properties.count, _properties.index.type, nullptr);
    } else {
        glDrawArrays(_properties.primitive_type, 0, _properties.count);
    }
}
