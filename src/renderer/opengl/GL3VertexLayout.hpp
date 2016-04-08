#pragma once

#include "renderer/VertexLayout.hpp"
#include "GL3BufferObject.hpp"

#include <vector>
#include <glad/glad.h>

struct Component {
    GLint attribute_location;
    GLenum data_type;
    GLint size;
    GLsizei stride;
    GLuint divisor;
    void* offset;

    GL3BufferObject* buffer;
};

class GL3VertexLayout : public VertexLayout {
    std::vector<GL3BufferObject *> _attachedBuffers;

    std::vector<Component> _components;

    bool _haveIndexBuffer;
    BufferIndex _indexBuffer;

    GLuint _vaoHandle;
public:
    GL3VertexLayout();
    virtual ~GL3VertexLayout();

    virtual BufferIndex attachBufferObject(BufferObject *buffer) override;

    virtual void addComponent(AttributeType type, DataFormat format, size_t stride, BufferIndex source, size_t offset) override;

    virtual void addInstanceComponent(AttributeType type, DataFormat format, size_t instanceDivisor, size_t stride,
                                      BufferIndex source, size_t offset) override;

    virtual void setIndexBuffer(BufferIndex source) override;

    virtual void finalize() override;

    void bind();
};


