#pragma once

#include "renderer/RenderTarget.hpp"

#include <glad/glad.h>
#include <renderer/Texture.hpp>
#include "GL3Texture.hpp"
#include "GL3State.hpp"

class GL3RenderTarget final: public GL3Object, public RenderTarget {
    GLsizei _width;
    GLsizei _heigth;

    GLuint _renderFramebuffer;

    std::vector<std::unique_ptr<GL3Texture>> _colorTextures;

    std::unique_ptr<GL3Texture> _depthTexture;
 public:
    GL3RenderTarget(GL3Renderer* renderer, GLsizei width, GLsizei height, GLuint framebuffer);

    virtual ~GL3RenderTarget();

    virtual size_t getWidth() const override;

    virtual size_t getHeight() const override;

    virtual void copyToTexture(Texture* target) override;

    std::vector<TextureHandle*> getColorTextures() override;

    TextureHandle* getDepthTexture() override;

    bool hasDepthBuffer();

    void bindFramebuffer();

    void setDepthTexture(std::unique_ptr<GL3Texture>&& handle);

    void addColorTexture(std::unique_ptr<GL3Texture>&& handle);
};

