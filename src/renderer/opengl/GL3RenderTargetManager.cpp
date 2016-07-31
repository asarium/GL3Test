//
//

#include "GL3RenderTargetManager.hpp"
#include "GL3State.hpp"
#include "GL3Util.hpp"

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

void GL3RenderTargetManager::useRenderTarget(PointerWrapper<RenderTarget> target) {
    if (target == nullptr) {
        // This uses the id 0
        _currentRenderTarget = _defaultRenderTarget.get();
    } else {
        _currentRenderTarget = static_cast<GL3RenderTarget*>(&target);
    }
    _currentRenderTarget->bindFramebuffer();
}

std::unique_ptr<RenderTarget> GL3RenderTargetManager::createRenderTarget(RenderTargetProperties&& properties) {
    // Make sure we don't interfere with any already bound framebuffer
    GLState->Framebuffer.pushBinding();
    GLuint framebuffer;


    glGenFramebuffers(1, &framebuffer);
    GLState->Framebuffer.bind(framebuffer);

    std::unique_ptr<GL3RenderTarget> target(new GL3RenderTarget(_renderer,
                                                                (GLsizei) properties.width,
                                                                (GLsizei) properties.height, framebuffer));

    std::vector<GLenum> draw_buffers;
    for (auto& texture : properties.color_buffers) {
        std::unique_ptr<GL3Texture> glTexture(static_cast<GL3Texture*>(texture.release()));
        auto attachment = (GLenum) (GL_COLOR_ATTACHMENT0 + draw_buffers.size());
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               attachment,
                               glTexture->getTarget(),
                               glTexture->getGLHandle(),
                               0);

        target->addColorTexture(std::move(glTexture));
        draw_buffers.push_back(attachment);
    }

    if (draw_buffers.empty()) {
        // If there are no color buffers, don't use it
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    } else {
        glDrawBuffers((GLsizei) draw_buffers.size(), draw_buffers.data());
    }

    if (properties.depth_texture) {
        std::unique_ptr<GL3Texture> glTexture(static_cast<GL3Texture*>(properties.depth_texture.release()));
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, glTexture->getTarget(), glTexture->getGLHandle(), 0);

        target->setDepthTexture(std::move(glTexture));
    }

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
