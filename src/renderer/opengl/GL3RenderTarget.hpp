#pragma once

#include "renderer/RenderTarget.hpp"

#include <glad/glad.h>

class GL3RenderTarget : public RenderTarget {
    size_t _width;
    size_t _heigth;

    GLuint _renderFramebuffer;
    GLuint _colorTexture;
    GLuint _depthRenderBuffer;
public:
    GL3RenderTarget(size_t width, size_t height, GLuint framebuffer, GLuint colorTexture, GLuint depthRenderBuffer);

    virtual ~GL3RenderTarget();

    virtual size_t getWidth() override;

    virtual size_t getHeight() override;

    void bindFramebuffer();

    GLuint getTextureHandle();
};


