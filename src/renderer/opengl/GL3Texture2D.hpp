#pragma once

#include "renderer/Texture2D.hpp"

#include <glad/glad.h>
#include <util/Assertion.hpp>
#include "GL3Object.hpp"

struct TextureProperties {
    GLenum internal_format;

    bool is_compressed;
    GLenum format;

    GLsizei width;
    GLsizei height;
};

class GL3Texture2D : public Texture2D {
    TextureProperties _props;
    GLuint _textureHandle;
public:
    GL3Texture2D();
    explicit GL3Texture2D(GLuint handle);
    virtual ~GL3Texture2D();

    void bind(int tex_unit = 0);

    void copyDataFromFramebuffer(GLsizei width, GLsizei height);

    virtual void initialize(size_t width, size_t height, TextureFormat format, void* data) override;

    virtual void updateData(void *data) override;

    virtual void initializeCompressed(size_t width, size_t height, CompressionFormat format, size_t data_size, void* data) override;

    virtual void updateCompressedData(size_t data_size, void *data) override;
};

