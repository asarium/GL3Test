//
//

#include "GL3RenderTargetManager.hpp"
#include "GL3State.hpp"

#include <iostream>
#include "GL3Renderer.hpp"

GL3RenderTargetManager::GL3RenderTargetManager(GL3Renderer *renderer) : GL3Object(renderer),
                                                                        _currentRenderTarget(nullptr) {
}

void GL3RenderTargetManager::useRenderTarget(RenderTarget *target) {
    if (target == nullptr) {
        // This uses the id 0
        _currentRenderTarget = _defaultRenderTarget.get();
    } else {
        _currentRenderTarget = static_cast<GL3RenderTarget *>(target);
    }
    _currentRenderTarget->bindFramebuffer();
}

std::unique_ptr<RenderTarget> GL3RenderTargetManager::createRenderTarget(size_t width, size_t height) {
    // Make sure we don't interfere with any already bound framebuffer
    GLState->Framebuffer.pushBinding();
    GLuint framebuffer;

    GLuint colorTexture;
    GLuint depthTexture;

    glGenFramebuffers(1, &framebuffer);
    GLState->Framebuffer.bind(framebuffer);

    glGenTextures(1, &colorTexture);
    GLState->Texture.bindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLint) width, (GLint) height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

    GLState->Texture.bindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &depthTexture);
    GLState->Texture.bindTexture(0, GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, (GLint)width, (GLint)height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    checkFrameBufferStatus();

    GLState->Framebuffer.popBinding();

    return std::unique_ptr<RenderTarget>(
        new GL3RenderTarget(width, height, framebuffer, colorTexture, depthTexture));
}

GL3RenderTarget *GL3RenderTargetManager::getCurrentRenderTarget() {
    return _currentRenderTarget;
}

void GL3RenderTargetManager::updateDefaultTarget(uint32_t width, uint32_t height) {
    _defaultRenderTarget.reset(new GL3RenderTarget((size_t) width, (size_t) height, 0, 0, 0));
    _currentRenderTarget = _defaultRenderTarget.get(); // By default the current render target is the screen
}

