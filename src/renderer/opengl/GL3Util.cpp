//
//

#include "GL3Renderer.hpp"

#include <iostream>

bool checkFrameBufferStatus() {
    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (status) {
        case GL_FRAMEBUFFER_COMPLETE:
            return true;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            std::cout << "FBO error: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"
                << std::endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            std::cout << "FBO error: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"
                << std::endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            std::cout << "FBO error: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"
                << std::endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            std::cout << "FBO error: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"
                << std::endl;
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            std::cout << "FBO error: GL_FRAMEBUFFER_UNSUPPORTED"
                << std::endl;
            break;
        default:
            std::cout << "FBO error: Unknown error"
                << std::endl;
            break;
    }

    return false;
}

