//
//

#include "GL3RenderTarget.hpp"
#include "GL3State.hpp"
#include "GL3Texture2D.hpp"

GL3RenderTarget::GL3RenderTarget(size_t width, size_t height, GLuint framebuffer, GLuint colorTexture,
                                 GLuint depthRenderBuffer) : _width((GLsizei) width), _heigth((GLsizei) height),
                                                             _renderFramebuffer(framebuffer),
                                                             _colorTexture(colorTexture),
                                                             _depthRenderBuffer(depthRenderBuffer) {

}

GL3RenderTarget::~GL3RenderTarget() {
    if (glIsFramebuffer(_renderFramebuffer)) {
        glDeleteFramebuffers(1, &_renderFramebuffer);
    }

    if (glIsTexture(_colorTexture)) {
        glDeleteTextures(1, &_colorTexture);
    }

    if (glIsRenderbuffer(_depthRenderBuffer)) {
        glDeleteTextures(1, &_depthRenderBuffer);
    }
}

size_t GL3RenderTarget::getHeight() {
    return (size_t) _heigth;
}

size_t GL3RenderTarget::getWidth() {
    return (size_t) _width;
}

GLuint GL3RenderTarget::getTextureHandle() {
    return _colorTexture;
}

void GL3RenderTarget::bindFramebuffer() {
    GLState->Framebuffer.bind(_renderFramebuffer);
    glViewport(0, 0, (GLsizei) _width, (GLsizei) _heigth);
}

void GL3RenderTarget::copyToTexture(Texture2D *target) {
    auto glTexture = static_cast<GL3Texture2D *>(target);

    GLState->Framebuffer.pushBinding();

    GLState->Framebuffer.bind(_renderFramebuffer);
    glTexture->copyDataFromFramebuffer(_width, _heigth);

    GLState->Framebuffer.popBinding();
}
