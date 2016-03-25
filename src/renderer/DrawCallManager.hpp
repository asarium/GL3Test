#pragma once

#include "VertexLayout.hpp"
#include "ShaderProgram.hpp"
#include "DrawCall.hpp"
#include "ShaderParameters.hpp"

struct PipelineState {
    bool depth_test;
};

struct DrawCallProperties {
    ShaderProgram *shader;
    ShaderParameters *parameters;
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

class DrawCallManager {
public:
    virtual ~DrawCallManager() { }

    virtual std::unique_ptr<DrawCall> createDrawCall(const DrawCallProperties &props, PrimitiveType type,
                                                     size_t count) = 0;

    virtual std::unique_ptr<DrawCall> createIndexedCall(const DrawCallProperties &props, PrimitiveType type,
                                                        size_t count, IndexType indexType) = 0;
};