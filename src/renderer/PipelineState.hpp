#pragma once

#include "DrawCallManager.hpp"

enum class BlendFunction {
    None,
    Additive
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
};

class PipelineState {
public:
    virtual ~PipelineState() { }
};
