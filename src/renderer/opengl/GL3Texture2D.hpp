#pragma once

#include "renderer/Texture2D.hpp"

#include <glad/glad.h>

struct TextureProperties {
    GLenum internal_format;

    bool is_compressed;
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

    void copyDataFromFramebuffer(GLsizei width, GLsizei height);

    virtual void initialize(size_t width, size_t height, TextureFormat format, void* data) override;

    virtual void updateData(void *data) override;

    virtual void initializeCompressed(size_t width, size_t height, CompressionFormat format, size_t data_size, void* data) override;

    virtual void updateCompressedData(size_t data_size, void *data) override;
};

