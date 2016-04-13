#pragma once

#include "DrawCall.hpp"
#include "ShaderParameters.hpp"

#include <memory>

class VertexLayout;

class PipelineState;

enum class ShaderType {
    Mesh,
    LightedMesh,
    ShadowMesh,
    PointSprite,
    InstancedSprite
};

struct DrawCallProperties {
    VertexLayout *vertexLayout;
};

enum class PrimitiveType {
    Point,
    Triangle,
    TriangleStrip
};

enum class IndexType {
    Short,
    Integer
};

class DrawCallManager {
public:
    virtual ~DrawCallManager() { }

    virtual std::unique_ptr<DrawCall> createDrawCall(const DrawCallProperties &props, PrimitiveType type,
                                                     size_t offset, size_t count) = 0;

    virtual std::unique_ptr<DrawCall> createIndexedCall(const DrawCallProperties &props, PrimitiveType type,
                                                        size_t offset, size_t count, IndexType indexType) = 0;

    virtual std::unique_ptr<VariableDrawCall> createVariableDrawCall(const DrawCallProperties &props,
                                                                     PrimitiveType type) = 0;

    virtual std::unique_ptr<VariableDrawCall> createVariableIndexedCall(const DrawCallProperties &props,
                                                                        PrimitiveType type,
                                                                        IndexType indexType) = 0;


    virtual std::unique_ptr<InstancedDrawCall> createInstancedDrawCall(const DrawCallProperties &props, PrimitiveType type,
                                                     size_t offset, size_t count) = 0;

    virtual std::unique_ptr<InstancedDrawCall> createInstancedIndexedCall(const DrawCallProperties &props, PrimitiveType type,
                                                        size_t offset, size_t count, IndexType indexType) = 0;

    virtual std::unique_ptr<InstancedVariableDrawCall> createInstancedVariableDrawCall(const DrawCallProperties &props,
                                                                     PrimitiveType type) = 0;

    virtual std::unique_ptr<InstancedVariableDrawCall> createInstancedVariableIndexedCall(const DrawCallProperties &props,
                                                                        PrimitiveType type,
                                                                        IndexType indexType) = 0;
};
