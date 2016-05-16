#pragma once

#include "DrawCallManager.hpp"
#include "Enums.hpp"

enum class BlendFunction {
    None,
    Additive,
    AdditiveAlpha,
    Alpha
};

enum class DepthMode {
    None,
    Read,
    Write,
    ReadWrite
};

enum class ShaderType {
    Mesh,
    LightedMesh,
    ShadowMesh,
    PointSprite,
    InstancedSprite,
    HdrPostProcessing,
    HdrBrightpass,
    HdrBloom,
    LightingPass,
};

struct PipelineProperties {
    ShaderType shaderType;

    DepthMode depthMode;
    ComparisionFunction depthFunction;

    bool blending;
    BlendFunction blendFunction;


    PipelineProperties() : shaderType(ShaderType::Mesh), depthMode(DepthMode::None), depthFunction(ComparisionFunction::Less),
                           blending(false), blendFunction(BlendFunction::None) { }
};

class PipelineState {
public:
    virtual ~PipelineState() { }

    virtual void bind() = 0;
};
