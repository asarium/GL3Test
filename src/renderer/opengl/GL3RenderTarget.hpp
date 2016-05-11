#pragma once

#include "renderer/RenderTarget.hpp"

#include <glad/glad.h>
#include <renderer/Texture.hpp>
#include "GL3Texture.hpp"

class GL3RenderTarget final: public RenderTarget {
    GLsizei _width;
    GLsizei _heigth;

    GLuint _renderFramebuffer;

    GL3Texture _colorTexture;

    GL3Texture _depthTexture;
 public:
    GL3RenderTarget(GL3Renderer* renderer,
                    GLsizei width,
                    GLsizei height,
                    GLuint framebuffer,
                    GLuint colorTexture,
                    GLuint depthTexture);

    virtual ~GL3RenderTarget();

    virtual size_t getWidth() const override;

    virtual size_t getHeight() const override;

    virtual void copyToTexture(Texture* target) override;

    TextureHandle* getColorTexture() override;

    TextureHandle* getDepthTexture() override;

    bool hasDepthBuffer();

    void bindFramebuffer();
};

