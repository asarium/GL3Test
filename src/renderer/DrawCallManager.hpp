#pragma once

#include "DrawCall.hpp"
#include "ShaderParameters.hpp"

#include <memory>

class VertexLayout;

class PipelineState;

enum class PrimitiveType {
    Point,
    Triangle,
    TriangleStrip
};

enum class IndexType {
    None,
    Short,
    Integer
};

struct DrawCallCreateProperties {
    VertexLayout* vertexLayout;

    PrimitiveType primitive_type;

    size_t offset;
    size_t count;

    IndexType index_type;
};

class DrawCallManager {
 public:
    virtual ~DrawCallManager() { }

    virtual std::unique_ptr<DrawCall> createDrawCall(const DrawCallCreateProperties& props) = 0;

    virtual std::unique_ptr<VariableDrawCall> createVariableDrawCall(const DrawCallCreateProperties& props) = 0;


    virtual std::unique_ptr<InstancedDrawCall> createInstancedDrawCall(const DrawCallCreateProperties& props) = 0;

    virtual std::unique_ptr<InstancedVariableDrawCall>
        createInstancedVariableDrawCall(const DrawCallCreateProperties& props) = 0;
};
