#pragma once

enum class BlendFunction {
    None,
    Additive
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
