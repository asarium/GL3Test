#pragma once

#include "renderer/Texture2D.hpp"

#include <glad/glad.h>
#include "GL3Object.hpp"

#include <util/Assertion.hpp>
#include <util/UniqueHandle.hpp>

struct GL3TextureHandle
{
protected:
    GLenum _type;
    GLuint _handle;

public:
    GL3TextureHandle();
    explicit GL3TextureHandle(GLenum type, GLuint glHandle);

    void bind(uint32_t tex_unit = 0);

    void unbind(uint32_t tex_unit = 0);

    GLuint getGLHandle();

    GLenum getType();
};

struct GL3OwnedTextureHandle : GL3TextureHandle
{
public:
    GL3OwnedTextureHandle();
    GL3OwnedTextureHandle(GLenum type, GLuint glHandle);
    ~GL3OwnedTextureHandle();

    GL3OwnedTextureHandle(const GL3OwnedTextureHandle&) = delete;
    GL3OwnedTextureHandle& operator=(const GL3OwnedTextureHandle&) = delete;

    GL3OwnedTextureHandle(GL3OwnedTextureHandle&& other);
    GL3OwnedTextureHandle& operator=(GL3OwnedTextureHandle&& other);
};

struct TextureProperties {
    GLenum internal_format;

    bool is_compressed;
    GLenum format;

    GLsizei width;
    GLsizei height;
};

class GL3Texture2D final: public GL3Object, public Texture2D, public GL3OwnedTextureHandle {
    TextureProperties _props;

    int _nvgHandle;
 public:
    explicit GL3Texture2D(GL3Renderer* renderer);
    explicit GL3Texture2D(GL3Renderer* renderer, GLuint handle);
    ~GL3Texture2D();

    void copyDataFromFramebuffer(GLsizei width, GLsizei height);

    void updateSize(GLsizei width, GLsizei height);

    int getNanoVGHandle() override;

    void initialize(size_t width, size_t height, TextureFormat format, void* data) override;

    void updateData(void* data) override;

    void initializeCompressed(size_t width, size_t height, CompressionFormat format, size_t data_size, void* data) override;

    void updateCompressedData(size_t data_size, void* data) override;

    static std::unique_ptr<GL3Texture2D> createTexture(GL3Renderer* renderer);
};

