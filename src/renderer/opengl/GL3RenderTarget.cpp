//
//

#include "GL3RenderTarget.hpp"
#include "GL3State.hpp"
#include "GL3Texture.hpp"

GL3RenderTarget::GL3RenderTarget(GL3Renderer* renderer,
                                 GLsizei width,
                                 GLsizei height,
                                 GLuint framebuffer)
    : GL3Object(renderer), _width(width), _heigth(height), _renderFramebuffer(framebuffer) {

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

void GL3RenderTarget::copyToTexture(Texture* target) {
    auto glTexture = static_cast<GL3Texture*>(target);

    GLState->Framebuffer.pushBinding();

    GLState->Framebuffer.bind(_renderFramebuffer);
    glTexture->copyDataFromFramebuffer(_width, _heigth);

    GLState->Framebuffer.popBinding();
}

TextureHandle* GL3RenderTarget::getDepthTexture() {
    return _depthTexture.get();
}

bool GL3RenderTarget::hasDepthBuffer() {
    return (bool) _depthTexture;
}
std::vector<TextureHandle*> GL3RenderTarget::getColorTextures() {
    std::vector<TextureHandle*> handles;
    for (auto& tex : _colorTextures) {
        handles.push_back(tex.get());
    }
    return handles;
}
void GL3RenderTarget::setDepthTexture(GLuint handle) {
    _depthTexture.reset(new GL3Texture(_renderer, handle));
    _depthTexture->updateSize(_width, _heigth);
}
void GL3RenderTarget::addColorTexture(GLuint handle) {
    std::unique_ptr<GL3Texture> texture(new GL3Texture(_renderer, handle));
    texture->updateSize(_width, _heigth);

    _colorTextures.push_back(std::move(texture));
}
