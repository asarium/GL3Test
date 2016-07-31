#pragma once

#include "Enums.hpp"
#include "VertexLayout.hpp"

#include <util/HashUtil.hpp>

#include <glm/glm.hpp>

enum class ShaderType {
    Mesh = 0,
    LightedMesh,
    ShadowMesh,
    PointSprite,
    InstancedSprite,
    HdrPostProcessing,
    HdrBrightpass,
    HdrBloom,
    LightingPass,
    NanoVGShader,

    NUM_VALUES
};
HASHABLE_ENUMCLASS(ShaderType)

enum class ShaderFlags {
    None = 0,
    NanoVGEdgeAA = 1 << 0,
};
HASHABLE_ENUMCLASS(ShaderFlags)

template<>
struct BitOperationsTag<ShaderFlags> {
    static constexpr bool value = true;
};

enum class BlendFunction {
    None,
    Additive,
    AdditiveAlpha,
    Alpha,
    PremultAlpha
};

enum class DepthMode {
    None,
    Read,
    Write,
    ReadWrite
};

enum class CullFace {
    Front,
    Back,
    FrontAndBack
};

enum class FrontFace {
    CounterClockWise,
    ClockWise
};

enum class StencilOperation {
    Keep,
    Zero,
    Replace,
    Increment,
    IncrementWrap,
    Decrement,
    DecrementWrap,
    Invert
};

enum class PrimitiveType {
    Point,
    Triangle,
    TriangleStrip,
    TriangleFan
};

struct PipelineProperties {
    ShaderType shaderType;
    ShaderFlags shaderFlags;

    PrimitiveType primitive_type;
    VertexInputStateProperties vertexInput;

    DepthMode depthMode;
    ComparisionFunction depthFunction;

    bool enableBlending;
    BlendFunction blendFunction;

    bool enableFaceCulling;
    CullFace culledFace;
    FrontFace frontFace;

    bool enableScissor;

    bool enableStencil;
    uint32_t stencilMask;
    std::tuple<ComparisionFunction, uint32_t, uint32_t> stencilFunc;
    std::tuple<StencilOperation, StencilOperation, StencilOperation> frontStencilOp;
    std::tuple<StencilOperation, StencilOperation, StencilOperation> backStencilOp;

    glm::bvec4 colorMask;

    PipelineProperties()
        : shaderType(ShaderType::Mesh), shaderFlags(ShaderFlags::None), primitive_type(PrimitiveType::Triangle), depthMode(DepthMode::None),
          depthFunction(ComparisionFunction::Less),
          enableBlending(false), blendFunction(BlendFunction::None), enableFaceCulling(false),
          culledFace(CullFace::Back), frontFace(FrontFace::CounterClockWise), enableScissor(false),
          enableStencil(false), stencilMask(0xffffffff),
          stencilFunc(ComparisionFunction::Always, 0, 0xffffffff),
          frontStencilOp(std::make_tuple(StencilOperation::Keep, StencilOperation::Keep, StencilOperation::Keep)),
          backStencilOp(std::make_tuple(StencilOperation::Keep, StencilOperation::Keep, StencilOperation::Keep)),
          colorMask(true, true, true, true) { }

    void setStencilOp(const std::tuple<StencilOperation, StencilOperation, StencilOperation>& op) {
        frontStencilOp = op;
        backStencilOp = op;
    }
};

class PipelineState {
 public:
    virtual ~PipelineState() { }
};
