#pragma once

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
    bool depth_test;

    bool blending;
    BlendFunction blendFunction;
};

class PipelineState {
public:
    virtual ~PipelineState() { }
};
