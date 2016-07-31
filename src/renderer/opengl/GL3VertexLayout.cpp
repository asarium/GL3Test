//
//

#include <iostream>
#include "GL3VertexLayout.hpp"
#include "EnumTranslation.hpp"
#include "GL3State.hpp"

namespace {
GLenum convertIndexType(IndexType type) {
    switch (type) {
        case IndexType::Short:
            return GL_UNSIGNED_SHORT;
        case IndexType::Integer:
            return GL_UNSIGNED_INT;
        default:
            Assertion(false, "Unhandled index type detected!");
            return GL_UNSIGNED_INT;
    }
    return GL_UNSIGNED_INT;
}
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

GL3VertexArrayObject::GL3VertexArrayObject(const VertexArrayProperties& props,
                                           const std::vector<Component>& components)
    : _vaoHandle(0), _indexType(GL_NONE), _indexOffset(0) {
    glGenVertexArrays(1, &_vaoHandle);
    GLState->bindVertexArray(_vaoHandle);
    for (auto& comp : components) {
        GL3BufferObject* boundBuffer = nullptr;
        for (auto& buffer : props.bufferBindings) {
            if (buffer.binding == comp.buffer_binding) {
                boundBuffer = static_cast<GL3BufferObject*>(buffer.buffer);
                break;
            }
        }
        Assertion(boundBuffer != nullptr, "No buffer found for vertex attribute!");
        Assertion(boundBuffer->getType() == BufferType::Vertex, "Non vertex buffer used for vertex component!");
        boundBuffer->bind();

        glEnableVertexAttribArray(comp.attribute_location);
        glVertexAttribPointer(comp.attribute_location, comp.size, comp.data_type, GL_FALSE, comp.stride, comp.offset);
        glVertexAttribDivisor(comp.attribute_location, comp.divisor);
    }

    if (props.indexBuffer != nullptr) {
        auto glIndexBuffer = static_cast<GL3BufferObject*>(props.indexBuffer);
        Assertion(glIndexBuffer->getType() == BufferType::Index, "Non index buffer used as index buffer!");
        glIndexBuffer->bind();

        _indexType = convertIndexType(props.indexType);
        _indexOffset = static_cast<size_t>(props.indexOffset);
    }

    GLState->bindVertexArray(0);
    GLState->Buffer.bindArrayBuffer(0);
    GLState->Buffer.bindElementBuffer(0);
}
GL3VertexArrayObject::~GL3VertexArrayObject() {

}
void GL3VertexArrayObject::bind() {
    GLState->bindVertexArray(_vaoHandle);
}
void* GL3VertexArrayObject::computeIndexOffset(uint32_t firstIndex) {
    return reinterpret_cast<void*>(firstIndex * getTypeSize(_indexType) + _indexOffset);
}
GLenum GL3VertexArrayObject::getIndexType() {
    return _indexType;
}


GL3VertexInputState::GL3VertexInputState(const VertexInputStateProperties& props) {
    for (auto& attribute : props.components) {
        Component comp;
        comp.attribute_location =
            mapAttributeLocation(attribute.type); // Data type is also the bound attribute location

        switch (attribute.format) {
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
            case DataFormat::Float:
                comp.data_type = GL_FLOAT;
                comp.size = 1;
                break;
        }

        comp.offset = reinterpret_cast<void*>(attribute.offset);
        comp.buffer_binding = attribute.bufferBinding;

        bool bufferFound = false;
        for (auto& buffer : props.bufferBindings) {
            if (buffer.bufferBinding == attribute.bufferBinding) {
                comp.stride = static_cast<GLsizei>(buffer.stride);
                comp.divisor = buffer.instanced ? 1 : 0; // Per-instance data
                bufferFound = true;
                break;
            }
        }
        Assertion(bufferFound, "The buffer binding for an attribute could not be found!");

        _components.push_back(comp);
    }
}

GL3VertexInputState::~GL3VertexInputState() {

}
std::unique_ptr<VertexArrayObject> GL3VertexInputState::createArrayObject(const VertexArrayProperties& props) {
    return std::unique_ptr<VertexArrayObject>(new GL3VertexArrayObject(props, _components));
}
