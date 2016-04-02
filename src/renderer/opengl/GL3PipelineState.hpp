#pragma once

#include "renderer/PipelineState.hpp"
#include "GL3ShaderProgram.hpp"

struct GL3PipelineProperties {
    GL3ShaderProgram *shader;

    bool depth_test;
    DepthFunction depthFunction;

    bool blending;
    BlendFunction blendFunction;
};

class GL3PipelineState : public PipelineState {
    GL3PipelineProperties _props;
public:
    GL3PipelineState(const GL3PipelineProperties &props) : _props(props) { }

    virtual ~GL3PipelineState() { }

    void setupState(const GL3ShaderParameters* params);
};


