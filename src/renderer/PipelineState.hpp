#pragma once

#include "DrawCallManager.hpp"
#include "Enums.hpp"

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

struct PipelineProperties {
    ShaderType shaderType;

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
        : shaderType(ShaderType::Mesh), depthMode(DepthMode::None), depthFunction(ComparisionFunction::Less),
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

    virtual void bind() = 0;
};
