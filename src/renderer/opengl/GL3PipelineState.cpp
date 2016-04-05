//
//

#include "GL3PipelineState.hpp"
#include "GL3State.hpp"

#include <glad/glad.h>

#include <util/Assertion.hpp>

void GL3PipelineState::setupState(const GL3ShaderParameters *parms) {
    _props.shader->bindAndSetParameters(parms);

    switch (_props.depthMode) {
        case DepthMode::None:
            // Disable read and write
            GLState->setDepthTest(false);
            GLState->setDepthMask(false);
            break;
        case DepthMode::Read:
            // Enable read and disable write
            GLState->setDepthTest(true);
            GLState->setDepthMask(false);
            break;
        case DepthMode::Write:
            // Disable read and enable write
            GLState->setDepthTest(false);
            GLState->setDepthMask(true);
            break;
        case DepthMode::ReadWrite:
            // Enable read and write
            GLState->setDepthTest(true);
            GLState->setDepthMask(true);
            break;
    }

    GLState->setDepthFunc(_props.depthFunction);

    GLState->setBlendMode(_props.blending);
    GLState->setBlendFunc(_props.blendFunction);
}

