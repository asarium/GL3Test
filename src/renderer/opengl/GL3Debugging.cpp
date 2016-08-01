//
//

#include "GL3Debugging.hpp"

#include <glad/glad.h>
#include <util/Assertion.hpp>

void GL3Debugging::addMessage(DebugSeverity severity, const std::string& message) {
    if (GLAD_GL_ARB_debug_output) {
        GLenum glSeverity;
        switch (severity) {
            case DebugSeverity::Low:
                glSeverity = GL_DEBUG_SEVERITY_LOW_ARB;
                break;
            case DebugSeverity::Medium:
                glSeverity = GL_DEBUG_SEVERITY_MEDIUM_ARB;
                break;
            case DebugSeverity::High:
                glSeverity = GL_DEBUG_SEVERITY_HIGH_ARB;
                break;
            default:
                Assertion(false, "Unhandled enum value!");
                return;
        }

        glDebugMessageInsertARB(GL_DEBUG_SOURCE_APPLICATION_ARB,
                                GL_DEBUG_TYPE_OTHER_ARB,
                                0,
                                glSeverity,
                                (GLsizei) message.size(),
                                message.c_str());
    }
}
void GL3Debugging::pushGroup(const std::string& name) {
    if (GLAD_GL_KHR_debug) {
        glPushDebugGroupKHR(GL_DEBUG_SOURCE_APPLICATION_KHR, 0, (GLsizei) name.size(), name.c_str());
    }
}
void GL3Debugging::popGroup() {
    if (GLAD_GL_KHR_debug) {
        glPopDebugGroupKHR();
    }
}
