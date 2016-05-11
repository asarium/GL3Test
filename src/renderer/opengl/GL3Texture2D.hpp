#pragma once

#include "renderer/Texture2D.hpp"

#include <glad/glad.h>
#include "GL3Object.hpp"

#include <util/Assertion.hpp>
#include <util/UniqueHandle.hpp>

#include <gli/texture.hpp>

struct GL3TextureHandle
{
protected:
    GLenum _target;
    GLuint _handle;

public:
    GL3TextureHandle();
    explicit GL3TextureHandle(GLenum type, GLuint glHandle);

    void bind(uint32_t tex_unit = 0);

    void unbind(uint32_t tex_unit = 0);

    GLuint getGLHandle();

    GLenum getTarget();
};

struct GL3OwnedTextureHandle : GL3TextureHandle
{
 protected:
    void reset(GLenum target, GLuint handle);
public:
    GL3OwnedTextureHandle();
    GL3OwnedTextureHandle(GLenum type, GLuint glHandle);
    ~GL3OwnedTextureHandle();

    GL3OwnedTextureHandle(const GL3OwnedTextureHandle&) = delete;
    GL3OwnedTextureHandle& operator=(const GL3OwnedTextureHandle&) = delete;

    GL3OwnedTextureHandle(GL3OwnedTextureHandle&& other);
    GL3OwnedTextureHandle& operator=(GL3OwnedTextureHandle&& other);
};

class GL3Texture2D final: public GL3Object, public Texture2D, public GL3OwnedTextureHandle {
    gli::texture::extent_type _extent;
    gli::texture::format_type _format;

    int _nvgHandle;
 public:
    explicit GL3Texture2D(GL3Renderer* renderer);
    explicit GL3Texture2D(GL3Renderer* renderer, GLuint handle);
    ~GL3Texture2D();

    void copyDataFromFramebuffer(GLsizei width, GLsizei height);

    void updateSize(GLsizei width, GLsizei height);

    int getNanoVGHandle() override;

    void initialize(const gli::texture& texture) override;

    static std::unique_ptr<GL3Texture2D> createTexture(GL3Renderer* renderer);
};

