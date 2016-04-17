#pragma once

#include "renderer/Texture2D.hpp"

#include <glad/glad.h>
#include "GL3Object.hpp"

#include <util/Assertion.hpp>
#include <util/UniqueHandle.hpp>

struct TextureProperties {
    GLenum internal_format;

    bool is_compressed;
    GLenum format;

    GLsizei width;
    GLsizei height;
};

struct GL3TextureDeleter {
    typedef UniqueHandle<GLuint, 0> pointer;
    void operator()(pointer p) {
        glDeleteTextures(1, &p);
    }
};
typedef std::unique_ptr<GLuint, GL3TextureDeleter> Gl3TextureHandle;

class GL3Texture2D : public Texture2D {
    TextureProperties _props;
    Gl3TextureHandle _handle;
public:
    GL3Texture2D();
    explicit GL3Texture2D(GLuint handle);
    virtual ~GL3Texture2D();

    GLuint getHandle();

    void reset(GLuint handle = 0);

    void bind(int tex_unit = 0);

    void copyDataFromFramebuffer(GLsizei width, GLsizei height);

    virtual void initialize(size_t width, size_t height, TextureFormat format, void* data) override;

    virtual void updateData(void *data) override;

    virtual void initializeCompressed(size_t width, size_t height, CompressionFormat format, size_t data_size, void* data) override;

    virtual void updateCompressedData(size_t data_size, void *data) override;

    static std::unique_ptr<GL3Texture2D> createTexture();
};

