#pragma once

#include "renderer/PipelineState.hpp"
#include "GL3ShaderProgram.hpp"

struct GL3PipelineProperties {
    GL3ShaderProgram* shader;

    DepthMode depthMode;
    DepthFunction depthFunction;

    bool blending;
    BlendFunction blendFunction;
};

class GL3PipelineState final: public PipelineState {
    GL3PipelineProperties _props;
 public:
    GL3PipelineState(const GL3PipelineProperties& props) : _props(props) { }

    virtual ~GL3PipelineState() { }

    virtual void bind() override;

    void setupState();
};


