//
//

#include <iostream>
#include "GL3VertexLayout.hpp"
#include "EnumTranslation.hpp"
#include "GL3State.hpp"

GL3VertexLayout::GL3VertexLayout() : _vaoHandle(0), _haveIndexBuffer(false) {

}

GL3VertexLayout::~GL3VertexLayout() {
    if (_vaoHandle != 0) {
        glDeleteVertexArrays(1, &_vaoHandle);
    }
}

VertexLayout::BufferIndex GL3VertexLayout::attachBufferObject(BufferObject *buffer) {
    _attachedBuffers.push_back(static_cast<GL3BufferObject *>(buffer));
    return _attachedBuffers.size() - 1;
}

void GL3VertexLayout::addComponent(AttributeType type, DataFormat format, size_t stride, BufferIndex source, size_t offset) {
    Component comp;
    comp.attribute_location = mapAttributeLocation(type); // Data type is also the bound attribute location

    switch (format) {
        case DataFormat::Vec4:
            comp.data_type = GL_FLOAT;
            comp.size = 4;
            break;
        case DataFormat::Vec3:
            comp.data_type = GL_FLOAT;
            comp.size = 3;
            break;
        case DataFormat::Vec2:
            comp.data_type = GL_FLOAT;
            comp.size = 2;
            break;
    }

    comp.stride = static_cast<GLsizei>(stride);
    comp.offset = reinterpret_cast<void*>(offset);
    comp.buffer = _attachedBuffers[source];

    _components.push_back(comp);
}

void GL3VertexLayout::setIndexBuffer(BufferIndex source) {
    _haveIndexBuffer = true;
    _indexBuffer = source;
}

void GL3VertexLayout::finalize() {
    glGenVertexArrays(1, &_vaoHandle);
    GLState->bindVertexArray(_vaoHandle);
    for (auto& comp : _components) {
        comp.buffer->bind();

        glEnableVertexAttribArray(comp.attribute_location);
        glVertexAttribPointer(comp.attribute_location, comp.size, comp.data_type, GL_FALSE, comp.stride, comp.offset);
    }

    if (_haveIndexBuffer) {
        _attachedBuffers[_indexBuffer]->bind();
    }

    GLState->bindVertexArray(0);
}

void GL3VertexLayout::bind() {
    GLState->bindVertexArray(_vaoHandle);
}
