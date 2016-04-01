#pragma once

#include "renderer/Texture2D.hpp"

#include <glad/glad.h>

struct TextureProperties {
    GLenum internal_format;
    GLenum format;

    GLsizei width;
    GLsizei height;
};

class GL3Texture2D : public Texture2D {
    GLuint _textureHandle;

    TextureProperties _props;
public:
    GL3Texture2D();
    virtual ~GL3Texture2D();

    void bind(int tex_unit = 0);

    virtual void initialize(size_t width, size_t height, TextureFormat format, void* data) override;
    virtual void updateDate(void* data) override;
};

