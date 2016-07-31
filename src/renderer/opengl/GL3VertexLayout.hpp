#pragma once

#include "renderer/VertexLayout.hpp"
#include "GL3BufferObject.hpp"

#include <vector>
#include <glad/glad.h>


struct Component {
    uint32_t buffer_binding;

    GLuint attribute_location;
    GLenum data_type;
    GLint size;
    GLsizei stride;
    GLuint divisor;
    void* offset;
};

class GL3VertexArrayObject final: public VertexArrayObject {
    GLuint _vaoHandle;

    GLenum _indexType;
    size_t _indexOffset;
 public:
    GL3VertexArrayObject(const VertexArrayProperties& props, const std::vector<Component>& components);
    virtual ~GL3VertexArrayObject() override;

    void bind();

    void* computeIndexOffset(uint32_t firstIndex);

    GLenum getIndexType();
};

class GL3VertexInputState final: public VertexInputState {
    std::vector<Component> _components;
 public:
    GL3VertexInputState(const VertexInputStateProperties& props);

    virtual ~GL3VertexInputState() override;

    virtual std::unique_ptr<VertexArrayObject> createArrayObject(const VertexArrayProperties& props) override;
};
