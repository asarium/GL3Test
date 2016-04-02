#pragma once

#include "DrawCallManager.hpp"

enum class BlendFunction {
    None,
    Additive
};

struct PipelineProperties {
    ShaderType shaderType;

    bool depth_test;

    bool blending;
    BlendFunction blendFunction;
};

class PipelineState {
public:
    virtual ~PipelineState() { }
};
