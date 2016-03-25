#pragma once

#include "renderer/DrawCallManager.hpp"
#include "GL3VertexLayout.hpp"
#include "GL3ShaderProgram.hpp"

struct GL3PipelineState {
    bool depth_test;
};

struct GL3DrawCallProperties {
    GL3ShaderProgram *shader;
    GL3ShaderParameters* parameters;
    GL3VertexLayout *vertexLayout;
    GL3PipelineState state;

    GLenum primitive_type;
    GLsizei count;

    bool indexed;
    struct {
        GLenum type;
    } index;
};

class GL3DrawCallManager : public DrawCallManager {
public:
    virtual ~GL3DrawCallManager() {};

    virtual std::unique_ptr<DrawCall> createDrawCall(const DrawCallProperties &props, PrimitiveType type,
                                                     size_t count) override;

    virtual std::unique_ptr<DrawCall> createIndexedCall(const DrawCallProperties &props, PrimitiveType type,
                                                        size_t count, IndexType indexType) override;
};


