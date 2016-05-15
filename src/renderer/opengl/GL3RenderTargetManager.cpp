//
//

#include "GL3RenderTargetManager.hpp"
#include "GL3State.hpp"

#include <iostream>
#include <renderer/RenderTargetManager.hpp>
#include "GL3Renderer.hpp"

namespace {
GLenum convertColorFormat(ColorBufferFormat format) {
    switch (format) {
        case ColorBufferFormat::RGB:
            return GL_RGB;
        case ColorBufferFormat::RGBA:
            return GL_RGBA;
        case ColorBufferFormat::RGB16F:
            return GL_RGB16F;
        case ColorBufferFormat::RGBA16F:
            return GL_RGBA16F;
        case ColorBufferFormat::RGB32F:
            return GL_RGB32F;
        case ColorBufferFormat::RGBA32F:
            return GL_RGBA32F;
    }
    return GL_RGB;
}
}

GL3RenderTargetManager::GL3RenderTargetManager(GL3Renderer* renderer) : GL3Object(renderer),
                                                                        _currentRenderTarget(nullptr) {
}

void GL3RenderTargetManager::useRenderTarget(RenderTarget* target) {
    if (target == nullptr) {
        // This uses the id 0
        _currentRenderTarget = _defaultRenderTarget.get();
    } else {
        _currentRenderTarget = static_cast<GL3RenderTarget*>(target);
    }
    _currentRenderTarget->bindFramebuffer();
}

std::unique_ptr<RenderTarget> GL3RenderTargetManager::createRenderTarget(const RenderTargetProperties& properties) {
    // Make sure we don't interfere with any already bound framebuffer
    GLState->Framebuffer.pushBinding();
    GLuint framebuffer;


    glGenFramebuffers(1, &framebuffer);
    GLState->Framebuffer.bind(framebuffer);

    std::unique_ptr<GL3RenderTarget> target(new GL3RenderTarget(_renderer,
                                                                (GLsizei) properties.width,
                                                                (GLsizei) properties.height, framebuffer));

    std::vector<GLenum> draw_buffers;
    for (auto& colorFmt : properties.color_buffers) {
        auto internal_color = convertColorFormat(colorFmt);

        GLState->Texture.unbindAll();
        GLuint colorTexture;
        glGenTextures(1, &colorTexture);
        GLState->Texture.bindTexture(GL_TEXTURE_2D, colorTexture);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     internal_color,
                     (GLint) properties.width,
                     (GLint) properties.height,
                     0,
                     GL_RGB,
                     GL_FLOAT,
                     nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        target->addColorTexture(colorTexture);
        auto attachment = (GLenum) (GL_COLOR_ATTACHMENT0 + draw_buffers.size());
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               attachment,
                               GL_TEXTURE_2D,
                               colorTexture,
                               0);
        draw_buffers.push_back(attachment);
    }

    glDrawBuffers((GLsizei) draw_buffers.size(), draw_buffers.data());

    if (properties.depth.enable) {
        GLuint depthTexture = 0;
        glGenTextures(1, &depthTexture);
        GLState->Texture.bindTexture(0, GL_TEXTURE_2D, depthTexture);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_DEPTH_COMPONENT,
                     (GLint) properties.width,
                     (GLint) properties.height,
                     0,
                     GL_DEPTH_COMPONENT,
                     GL_FLOAT,
                     nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
        target->setDepthTexture(depthTexture);
    }
    GLState->Texture.unbindAll();

    checkFrameBufferStatus();

    GLState->Framebuffer.popBinding();

    return std::unique_ptr<RenderTarget>(target.release());
}

RenderTarget* GL3RenderTargetManager::getCurrentRenderTarget() {
    return _currentRenderTarget;
}

void GL3RenderTargetManager::updateDefaultTarget(uint32_t width, uint32_t height) {
    _defaultRenderTarget.reset(new GL3RenderTarget(_renderer, width, height, 0));
    _currentRenderTarget = _defaultRenderTarget.get(); // By default the current render target is the screen
}
void GL3RenderTargetManager::pushRenderTargetBinding() {
    _renderTargetStack.push(_currentRenderTarget);
}
void GL3RenderTargetManager::popRenderTargetBinding() {
    auto oldTarget = _renderTargetStack.top();
    _renderTargetStack.pop();

    useRenderTarget(oldTarget);
}
