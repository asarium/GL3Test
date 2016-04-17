#pragma once

#include "renderer/RenderTarget.hpp"

#include <glad/glad.h>
#include <renderer/Texture2D.hpp>
#include "GL3Texture2D.hpp"

class GL3RenderTarget : public RenderTarget
{
    GLsizei _width;
    GLsizei _heigth;

    GLuint _renderFramebuffer;
    
    GL3Texture2D _colorTexture;

    GL3Texture2D _depthTexture;
public:
    GL3RenderTarget(size_t width, size_t height, GLuint framebuffer, GLuint colorTexture, GLuint depthTexture);

    virtual ~GL3RenderTarget();

    virtual size_t getWidth() const override;

    virtual size_t getHeight() const override;

    virtual void copyToTexture(Texture2D* target) override;

    Texture2DHandle* getColorTexture() override;

    Texture2DHandle* getDepthTexture() override;

    void bindFramebuffer();
};

