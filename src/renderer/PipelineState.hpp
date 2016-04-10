#pragma once

#include "DrawCallManager.hpp"

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

enum class DepthFunction {
    Never,
    Always,
    Less,
    Greater,
    Equal,
    NotEqual,
    LessOrEqual,
    GreaterOrEqual
};

struct PipelineProperties {
    ShaderType shaderType;

    DepthMode depthMode;
    DepthFunction depthFunction;

    bool blending;
    BlendFunction blendFunction;


    PipelineProperties() : shaderType(ShaderType::Mesh), depthMode(DepthMode::None), depthFunction(DepthFunction::Less),
                           blending(false), blendFunction(BlendFunction::None) { }
};

class PipelineState {
public:
    virtual ~PipelineState() { }
};
