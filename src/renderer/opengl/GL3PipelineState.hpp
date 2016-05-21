#pragma once

#include "renderer/PipelineState.hpp"
#include "Enums.hpp"
#include "GL3Object.hpp"

#include <glad/glad.h>

struct GL3PipelineProperties {
    ShaderType shaderType;

    DepthMode depthMode;
    ComparisionFunction depthFunction;

    bool enableBlending;
    BlendFunction blendFunction;

    bool enableFaceCulling;
    GLenum culledFace;
    GLenum frontFace;

    bool enableScissor;

    bool enableStencil;
    uint32_t stencilMask;

    std::tuple<GLenum, GLenum, GLenum> frontStencilOp;
    std::tuple<GLenum, GLenum, GLenum> backStencilOp;
    std::tuple<GLenum, uint32_t, uint32_t> stencilFunc;

    glm::bvec4 colorMask;
};

class GL3PipelineState final: public GL3Object, public PipelineState {
    GL3PipelineProperties _props;
 public:
    GL3PipelineState(GL3Renderer* renderer, const PipelineProperties& props);

    virtual ~GL3PipelineState() { }

    virtual void bind() override;

    void setupState();
};


