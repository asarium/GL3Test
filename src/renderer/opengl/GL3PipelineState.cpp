//
//

#include "GL3PipelineState.hpp"
#include "GL3State.hpp"

#include <glad/glad.h>

void GL3PipelineState::setupState() {
    GLState->setDepthTest(_props.depth_test);
}

