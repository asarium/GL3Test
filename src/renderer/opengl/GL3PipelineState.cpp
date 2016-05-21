//
//

#include "GL3PipelineState.hpp"
#include "GL3State.hpp"
#include "GL3Renderer.hpp"

#include <glad/glad.h>

#include <util/Assertion.hpp>
#include "EnumTranslation.hpp"

namespace
{
    GLenum convertCulledFace(const CullFace cull_face)
    {
        switch (cull_face)
        {
        case CullFace::Front:
            return GL_FRONT;
        case CullFace::Back:
            return GL_BACK;
        case CullFace::FrontAndBack:
            return GL_FRONT_AND_BACK;
        default:
            Assertion(false, "Unhandled enum value encountered!");
            return GL_NONE;
        }
    }

    GLenum convertFrontFace(const FrontFace front_face)
    {
        switch (front_face)
        {
        case FrontFace::CounterClockWise:
            return GL_CCW;
        case FrontFace::ClockWise:
            return GL_CW;
        default:
            Assertion(false, "Unhandled enum value encountered!");
            return GL_NONE;
        }
    }

    GLenum convertStencilOp(const StencilOperation stencil_op)
    {
        switch (stencil_op)
        {
        case StencilOperation::Keep:
            return GL_KEEP;
        case StencilOperation::Zero:
            return GL_ZERO;
        case StencilOperation::Replace:
            return GL_REPLACE;
        case StencilOperation::Increment:
            return GL_INCR;
        case StencilOperation::IncrementWrap:
            return GL_INCR_WRAP;
        case StencilOperation::Decrement:
            return GL_DECR;
        case StencilOperation::DecrementWrap:
            return GL_DECR_WRAP;
        case StencilOperation::Invert:
            return GL_INVERT;
        default:
            Assertion(false, "Unhandled enum value encountered!");
            return GL_NONE;
        }
    }
}

void GL3PipelineState::setupState()
{
    _renderer->getShaderManager()->bindProgram(_props.shaderType);

    switch (_props.depthMode)
    {
    case DepthMode::None:
        // Disable read and write
        GLState->setDepthTest(false);
        GLState->setDepthMask(false);
        break;
    case DepthMode::Read:
        // Enable read and disable write
        GLState->setDepthTest(true);
        GLState->setDepthFunc(_props.depthFunction);
        GLState->setDepthMask(false);
        break;
    case DepthMode::Write:
        // Disable read and enable write
        // "Even if the depth buffer exists and the depth mask is non-zero, the depth buffer is not updated if the depth test is disabled."
        // From the OpenGL Spec. That means that we have to enable the depth test even though we don't want to use it
        // instead we set the depth function to always accept which should have the same effect
        GLState->setDepthTest(true);
        GLState->setDepthFunc(ComparisionFunction::Always);
        GLState->setDepthMask(true);
        break;
    case DepthMode::ReadWrite:
        // Enable read and write
        GLState->setDepthTest(true);
        GLState->setDepthFunc(_props.depthFunction);
        GLState->setDepthMask(true);
        break;
    }

    GLState->setBlendMode(_props.enableBlending);
    GLState->setBlendFunc(_props.blendFunction);

    GLState->Stencil.setStencilTest(_props.enableStencil);
    GLState->Stencil.setStencilMask(_props.stencilMask);
    GLState->Stencil.setStencilFunc(_props.stencilFunc);
    GLState->Stencil.setFrontStencilOp(_props.frontStencilOp);
    GLState->Stencil.setBackStencilOp(_props.backStencilOp);

    GLState->setScissorTest(_props.enableScissor);

    GLState->CullFace.setFaceCulling(_props.enableFaceCulling);
    GLState->CullFace.setCulledFace(_props.culledFace);
    GLState->CullFace.setFrontFace(_props.frontFace);

    GLState->setColorMask(_props.colorMask);
}

GL3PipelineState::GL3PipelineState(GL3Renderer* renderer, const PipelineProperties& props): GL3Object(renderer)
{
    _props.shaderType = props.shaderType;

    _props.depthMode = props.depthMode;
    _props.depthFunction = props.depthFunction;

    _props.enableBlending = props.enableBlending;
    _props.blendFunction = props.blendFunction;

    _props.enableFaceCulling = props.enableFaceCulling;
    _props.culledFace = convertCulledFace(props.culledFace);
    _props.frontFace = convertFrontFace(props.frontFace);

    _props.enableScissor = props.enableScissor;

    _props.enableStencil = props.enableStencil;
    _props.stencilMask = props.stencilMask;

    _props.frontStencilOp = std::make_tuple(convertStencilOp(std::get<0>(props.frontStencilOp)),
                                       convertStencilOp(std::get<1>(props.frontStencilOp)),
                                       convertStencilOp(std::get<2>(props.frontStencilOp)));
    _props.backStencilOp = std::make_tuple(convertStencilOp(std::get<0>(props.backStencilOp)),
                                       convertStencilOp(std::get<1>(props.backStencilOp)),
                                       convertStencilOp(std::get<2>(props.backStencilOp)));

    _props.stencilFunc = std::make_tuple(convertComparisionFunction(std::get<0>(props.stencilFunc)), std::get<1>(props.stencilFunc), std::get<2>(props.stencilFunc));

    _props.colorMask = props.colorMask;

    // Issue a dummy draw call for less frame drops later (if the driver actually uses this information)
    setupState();
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 0);
    glDeleteVertexArrays(1, &vao);
}

void GL3PipelineState::bind()
{
    setupState();
}
