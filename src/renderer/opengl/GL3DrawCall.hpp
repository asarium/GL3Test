#pragma once

#include "renderer/DrawCall.hpp"
#include "GL3ShaderProgram.hpp"
#include "GL3VertexLayout.hpp"

struct GL3PipelineState {
    bool depth_test;
};

struct GL3DrawCallProperties {
    GL3ShaderProgram *shader;
    GL3VertexLayout *vertexLayout;
    GL3PipelineState state;
};

class GL3DrawCall : public DrawCall {
    GL3DrawCallProperties _properties;

    void setGLState();
public:
    GL3DrawCall(const GL3DrawCallProperties &);

    ~GL3DrawCall();

    virtual void draw(PrimitiveType type, size_t count) override;

    virtual void drawIndexed(PrimitiveType type, size_t count, IndexType indexType) override;
};


