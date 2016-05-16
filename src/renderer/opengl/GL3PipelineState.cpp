//
//

#include "GL3PipelineState.hpp"
#include "GL3State.hpp"
#include "GL3Renderer.hpp"

#include <glad/glad.h>

#include <util/Assertion.hpp>

void GL3PipelineState::setupState() {
    _renderer->getShaderManager()->bindProgram(_props.shaderType);

    switch (_props.depthMode) {
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

    GLState->setBlendMode(_props.blending);
    GLState->setBlendFunc(_props.blendFunction);
}
void GL3PipelineState::bind() {
    setupState();
}


