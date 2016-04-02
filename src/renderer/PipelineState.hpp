#pragma once

#include "DrawCallManager.hpp"

enum class BlendFunction {
    None,
    Additive
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

    bool depth_test;
    DepthFunction depthFunction;

    bool blending;
    BlendFunction blendFunction;
};

class PipelineState {
public:
    virtual ~PipelineState() { }
};
