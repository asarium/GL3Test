//
//

#include "GL3Texture.hpp"
#include "GL3State.hpp"
#include "GL3Renderer.hpp"

#include <renderer/Exceptions.hpp>

#include <gli/gli.hpp>

GL3TextureHandle::GL3TextureHandle() : _target(GL_TEXTURE_2D), _handle(0) {
}

GL3TextureHandle::GL3TextureHandle(GLenum type, GLuint glHandle) : _target(type), _handle(glHandle) {
}

void GL3TextureHandle::bind(uint32_t tex_unit) {
    GLState->Texture.bindTexture(tex_unit, _target, _handle);
}

void GL3TextureHandle::unbind(uint32_t tex_unit) {
    GLState->Texture.bindTexture(tex_unit, _target, 0);
}

GLuint GL3TextureHandle::getGLHandle() {
    return _handle;
}

GLenum GL3TextureHandle::getTarget() {
    return _target;
}

GL3OwnedTextureHandle::GL3OwnedTextureHandle() : GL3TextureHandle() {
}

GL3OwnedTextureHandle::GL3OwnedTextureHandle(GLenum type, GLuint glHandle) : GL3TextureHandle(type, glHandle) {
}

GL3OwnedTextureHandle::~GL3OwnedTextureHandle() {
    reset(_target, 0);
}

GL3OwnedTextureHandle::GL3OwnedTextureHandle(GL3OwnedTextureHandle&& other) : GL3TextureHandle(other._target, 0) {
    *this = std::move(other);
}

GL3OwnedTextureHandle& GL3OwnedTextureHandle::operator=(GL3OwnedTextureHandle&& other) {
    _target = other._target;
    _handle = other._handle;

    other._handle = 0;

    return *this;
}
void GL3OwnedTextureHandle::reset(GLenum target, GLuint handle) {
    if (_handle != 0 && _handle != handle) {
        glDeleteTextures(1, &_handle);
        _handle = 0;
    }

    _target = target;
    _handle = handle;
}

GL3Texture::GL3Texture(GL3Renderer* renderer)
    : GL3Object(renderer), GL3OwnedTextureHandle(GL_TEXTURE_2D, 0), _nvgHandle(0) {
}

GL3Texture::GL3Texture(GL3Renderer* renderer, GLuint handle)
    : GL3Object(renderer), GL3OwnedTextureHandle(GL_TEXTURE_2D, handle), _nvgHandle(0) {
}

GL3Texture::~GL3Texture() {
}

void GL3Texture::copyDataFromFramebuffer(GLsizei width, GLsizei height) {
    reset(GL_TEXTURE_2D, _handle);
    this->bind();

    _format = gli::FORMAT_RGBA8_UNORM_PACK8;
    _extent.x = width;
    _extent.y = height;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0, width, height, 0);
    GLState->Texture.bindTexture(0, GL_TEXTURE_2D, 0);
}
std::unique_ptr<GL3Texture> GL3Texture::createTexture(GL3Renderer* renderer) {
    GLuint name;
    glGenTextures(1, &name);
    return std::unique_ptr<GL3Texture>(new GL3Texture(renderer, name));
}

void GL3Texture::updateSize(GLsizei width, GLsizei height) {
    _extent.x = width;
    _extent.y = height;

    // Implicitly makes this texture 2D
    reset(GL_TEXTURE_2D, _handle);
}
int GL3Texture::getNanoVGHandle() {
    Assertion(_handle != 0, "Trying to get NanoVG handle from invalid texture!");

    if (_nvgHandle > 0) {
        return _nvgHandle;
    }

    _nvgHandle = _renderer->getNanoVGImageHandle(_handle, _extent.x, _extent.y);
    return _nvgHandle;
}
void GL3Texture::initialize(const gli::texture& texture) {
    gli::gl GL(gli::gl::PROFILE_GL33);
    gli::gl::format const format = GL.translate(texture.format(), texture.swizzles());
    GLenum target = GL.translate(texture.target());

    reset(target, _handle);
    bind(0);

    glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(texture.levels() - 1));
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_R, format.Swizzles[0]);
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_G, format.Swizzles[1]);
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_B, format.Swizzles[2]);
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_A, format.Swizzles[3]);

    GLsizei const faceTotal = static_cast<GLsizei>(texture.layers() * texture.faces());

    // First allocate store for all mipmap levels
    for (std::size_t level = 0; level < texture.levels(); ++level) {
        auto size = texture.extent(level);

        switch (texture.target()) {
            case gli::TARGET_1D:
                glTexImage1D(target, (GLint) level, format.Internal, size.x, 0, format.External, format.Type, nullptr);
                break;
            case gli::TARGET_1D_ARRAY:
            case gli::TARGET_2D:
            case gli::TARGET_CUBE:
                glTexImage2D(target,
                             (GLint) level,
                             format.Internal,
                             size.x,
                             texture.target() == gli::TARGET_2D ? size.y : faceTotal,
                             0,
                             format.External,
                             format.Type,
                             nullptr);
                break;
            case gli::TARGET_2D_ARRAY:
            case gli::TARGET_3D:
            case gli::TARGET_CUBE_ARRAY:
                glTexImage3D(target,
                             (GLint) level,
                             format.Internal,
                             size.x,
                             size.y,
                             texture.target() == gli::TARGET_3D ? size.z : faceTotal,
                             0,
                             format.External,
                             format.Type,
                             nullptr);
                break;
            default:
                Assertion(false, "Unknown texture target encountered!");
                break;
        }
    }

    for (std::size_t Layer = 0; Layer < texture.layers(); ++Layer) {
        for (std::size_t Face = 0; Face < texture.faces(); ++Face) {
            for (std::size_t Level = 0; Level < texture.levels(); ++Level) {
                GLsizei const LayerGL = static_cast<GLsizei>(Layer);
                glm::tvec3<GLsizei> Extent(texture.extent(Level));
                target = gli::is_target_cube(texture.target())
                         ? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face)
                         : target;

                switch (texture.target()) {
                    case gli::TARGET_1D:
                        if (gli::is_compressed(texture.format()))
                            glCompressedTexSubImage1D(
                                target, static_cast<GLint>(Level), 0, Extent.x,
                                format.Internal, static_cast<GLsizei>(texture.size(Level)),
                                texture.data(Layer, Face, Level));
                        else
                            glTexSubImage1D(
                                target, static_cast<GLint>(Level), 0, Extent.x,
                                format.External, format.Type,
                                texture.data(Layer, Face, Level));
                        break;
                    case gli::TARGET_1D_ARRAY:
                    case gli::TARGET_2D:
                    case gli::TARGET_CUBE:
                        if (gli::is_compressed(texture.format()))
                            glCompressedTexSubImage2D(
                                target, static_cast<GLint>(Level),
                                0, 0,
                                Extent.x,
                                texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
                                format.Internal, static_cast<GLsizei>(texture.size(Level)),
                                texture.data(Layer, Face, Level));
                        else
                            glTexSubImage2D(
                                target, static_cast<GLint>(Level),
                                0, 0,
                                Extent.x,
                                texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
                                format.External, format.Type,
                                texture.data(Layer, Face, Level));
                        break;
                    case gli::TARGET_2D_ARRAY:
                    case gli::TARGET_3D:
                    case gli::TARGET_CUBE_ARRAY:
                        if (gli::is_compressed(texture.format()))
                            glCompressedTexSubImage3D(
                                target, static_cast<GLint>(Level),
                                0, 0, 0,
                                Extent.x, Extent.y,
                                texture.target() == gli::TARGET_3D ? Extent.z : LayerGL,
                                format.Internal, static_cast<GLsizei>(texture.size(Level)),
                                texture.data(Layer, Face, Level));
                        else
                            glTexSubImage3D(
                                target, static_cast<GLint>(Level),
                                0, 0, 0,
                                Extent.x, Extent.y,
                                texture.target() == gli::TARGET_3D ? Extent.z : LayerGL,
                                format.External, format.Type,
                                texture.data(Layer, Face, Level));
                        break;
                    default:
                        Assertion(false, "Unknown texture target encountered!");
                        break;
                }
            }
        }
    }

    _extent = texture.extent(0);
    _format = texture.format();
}
