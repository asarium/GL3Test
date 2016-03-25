#pragma once

#include "ShaderProgram.hpp"
#include "VertexLayout.hpp"

struct PipelineState {
    bool depth_test;
};

struct DrawCallProperties {
    ShaderProgram *shader;
    VertexLayout *vertexLayout;
    PipelineState state;
};

enum class PrimitiveType {
    Triangle
};

enum class IndexType {
    Short,
    Integer
};

class DrawCall {
public:
    virtual ~DrawCall() { }

    virtual void draw(PrimitiveType type, size_t count) = 0;

    virtual void drawIndexed(PrimitiveType type, size_t count, IndexType indexType) = 0;
};
