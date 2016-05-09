#pragma once

#include "renderer/PipelineState.hpp"
#include "Enums.hpp"
#include "GL3Object.hpp"

#include <glad/glad.h>

struct GL3PipelineProperties {
    GL3ShaderType shaderType;

    DepthMode depthMode;
    DepthFunction depthFunction;

    bool blending;
    BlendFunction blendFunction;
};

class GL3PipelineState final: public GL3Object, public PipelineState {
    GL3PipelineProperties _props;
 public:
    GL3PipelineState(GL3Renderer* renderer, const GL3PipelineProperties& props) : GL3Object(renderer), _props(props) { }

    virtual ~GL3PipelineState() { }

    virtual void bind() override;

    void setupState();
};


