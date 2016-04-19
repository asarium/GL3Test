//
//

#include "GL3RenderTarget.hpp"
#include "GL3State.hpp"
#include "GL3Texture2D.hpp"

GL3RenderTarget::GL3RenderTarget(GL3Renderer* renderer,
                                 GLsizei width,
                                 GLsizei height,
                                 GLuint framebuffer,
                                 GLuint colorTexture,
                                 GLuint depthTexture) : _width(width), _heigth(height),
                                                        _renderFramebuffer(framebuffer),
                                                        _colorTexture(renderer, colorTexture),
                                                        _depthTexture(renderer, depthTexture) {
    _colorTexture.updateSize(width, height);
    _depthTexture.updateSize(width, height);
}

GL3RenderTarget::~GL3RenderTarget() {
    if (glIsFramebuffer(_renderFramebuffer)) {
        glDeleteFramebuffers(1, &_renderFramebuffer);
    }
}

size_t GL3RenderTarget::getHeight() const {
    return (size_t) _heigth;
}

size_t GL3RenderTarget::getWidth() const {
    return (size_t) _width;
}

void GL3RenderTarget::bindFramebuffer() {
    GLState->Framebuffer.bind(_renderFramebuffer);
    glViewport(0, 0, (GLsizei) _width, (GLsizei) _heigth);
}

void GL3RenderTarget::copyToTexture(Texture2D* target) {
    auto glTexture = static_cast<GL3Texture2D*>(target);

    GLState->Framebuffer.pushBinding();

    GLState->Framebuffer.bind(_renderFramebuffer);
    glTexture->copyDataFromFramebuffer(_width, _heigth);

    GLState->Framebuffer.popBinding();
}

Texture2DHandle* GL3RenderTarget::getColorTexture() {
    return &_colorTexture;
}

Texture2DHandle* GL3RenderTarget::getDepthTexture() {
    return &_depthTexture;
}

bool GL3RenderTarget::hasDepthBuffer() {
    return _depthTexture.getHandle() != 0;
}
