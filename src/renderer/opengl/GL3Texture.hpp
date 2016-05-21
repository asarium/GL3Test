#pragma once

#include "renderer/Texture.hpp"

#include <glad/glad.h>
#include "GL3Object.hpp"

#include <util/Assertion.hpp>
#include <util/UniqueHandle.hpp>

#include <gli/texture.hpp>

struct GL3TextureHandle {
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

struct GL3OwnedTextureHandle: GL3TextureHandle {
 protected:
    void reset(GLenum target, GLuint handle);
 public:
    GL3OwnedTextureHandle();
    GL3OwnedTextureHandle(GLenum type, GLuint glHandle);
    virtual ~GL3OwnedTextureHandle();

    GL3OwnedTextureHandle(const GL3OwnedTextureHandle&) = delete;
    GL3OwnedTextureHandle& operator=(const GL3OwnedTextureHandle&) = delete;

    GL3OwnedTextureHandle(GL3OwnedTextureHandle&& other);
    GL3OwnedTextureHandle& operator=(GL3OwnedTextureHandle&& other);
};

class GL3Texture final: public GL3Object, public Texture, public GL3OwnedTextureHandle {
    gli::extent3d _extent;
    gli::format _format;
    gli::swizzles _swizzles;

    void setFilterProperties(const FilterProperties& props) const;
 public:
    explicit GL3Texture(GL3Renderer* renderer);
    explicit GL3Texture(GL3Renderer* renderer, GLuint handle);
    ~GL3Texture();

    void copyDataFromFramebuffer(GLsizei width, GLsizei height);

    void allocate(const AllocationProperties& props) override;

    void initialize(const gli::texture& texture, const FilterProperties& filterProperties) override;

    void update(const gli::extent3d& position,
                const gli::extent3d& size, const gli::format dataFormat, const void* data) override;

    virtual gli::extent3d getSize() const override;

    static std::unique_ptr<GL3Texture> createTexture(GL3Renderer* renderer);
};

