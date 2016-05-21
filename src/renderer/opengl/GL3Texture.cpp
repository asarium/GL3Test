//
//

#include "GL3Texture.hpp"
#include "GL3State.hpp"
#include "GL3Renderer.hpp"
#include "EnumTranslation.hpp"

#include <renderer/Exceptions.hpp>

#include <gli/gli.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace {
GLenum convertWrapMode(WrapBehavior mode) {
    switch (mode) {
        case WrapBehavior::ClampToEdge:
            return GL_CLAMP_TO_EDGE;
        case WrapBehavior::ClampToBorder:
            return GL_CLAMP_TO_BORDER;
        case WrapBehavior::Repeat:
            return GL_REPEAT;
        default:
            Assertion(false, "Unhandled enum value!");
            return GL_NONE;
    }
}
GLenum convertCompareMode(TextureCompareMode mode) {
    switch (mode) {
        case TextureCompareMode::None:
            return GL_NONE;
        case TextureCompareMode::CompareRefToTexture:
            return GL_COMPARE_REF_TO_TEXTURE;
        default:
            Assertion(false, "Unhandled enum value!");
            return GL_NONE;
    }
}
GLenum convertFilterMode(FilterMode mode) {
    switch (mode) {
        case FilterMode::Nearest:
            return GL_NEAREST;
        case FilterMode::Linear:
            return GL_LINEAR;
        case FilterMode::NearestMipmapNearest:
            return GL_NEAREST_MIPMAP_NEAREST;
        case FilterMode::LinearMipmapNearest:
            return GL_LINEAR_MIPMAP_NEAREST;
        case FilterMode::NearestMipmapLinear:
            return GL_NEAREST_MIPMAP_LINEAR;
        case FilterMode::LinearMipmapLinear:
            return GL_LINEAR_MIPMAP_LINEAR;
        default:
            Assertion(false, "Unhandled enum value!");
            return GL_NONE;
    }
}
}

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
    reset(other._target, other._handle);

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
    : GL3Object(renderer), GL3OwnedTextureHandle(GL_TEXTURE_2D, 0), _swizzles(gli::SWIZZLE_RED,
                                                                              gli::SWIZZLE_GREEN,
                                                                              gli::SWIZZLE_BLUE,
                                                                              gli::SWIZZLE_ALPHA) {
}

GL3Texture::GL3Texture(GL3Renderer* renderer, GLuint handle)
    : GL3Object(renderer), GL3OwnedTextureHandle(GL_TEXTURE_2D, handle), _swizzles(gli::SWIZZLE_RED,
                                                                                   gli::SWIZZLE_GREEN,
                                                                                   gli::SWIZZLE_BLUE,
                                                                                   gli::SWIZZLE_ALPHA) {
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

void GL3Texture::allocate(const AllocationProperties& props) {
    gli::gl GL(gli::gl::PROFILE_GL33);
    gli::gl::format const format = GL.translate(props.format,
                                                gli::swizzles(gli::SWIZZLE_RED,
                                                              gli::SWIZZLE_GREEN,
                                                              gli::SWIZZLE_BLUE,
                                                              gli::SWIZZLE_ALPHA));
    GLenum target = GL.translate(props.target);

    reset(target, _handle);
    bind(0);

    glTexParameteri(_target, GL_TEXTURE_SWIZZLE_R, format.Swizzles[0]);
    glTexParameteri(_target, GL_TEXTURE_SWIZZLE_G, format.Swizzles[1]);
    glTexParameteri(_target, GL_TEXTURE_SWIZZLE_B, format.Swizzles[2]);
    glTexParameteri(_target, GL_TEXTURE_SWIZZLE_A, format.Swizzles[3]);

    setFilterProperties(props.filterProperties);

    glTexParameteri(_target, GL_TEXTURE_COMPARE_MODE, convertCompareMode(props.compare_mode));
    glTexParameteri(_target, GL_TEXTURE_COMPARE_FUNC, convertComparisionFunction(props.compare_func));

    switch (props.target) {
        case gli::TARGET_1D:
            glTexImage1D(target, 0, format.Internal, props.size.x, 0, format.External, format.Type, nullptr);
            break;
        case gli::TARGET_1D_ARRAY:
        case gli::TARGET_2D:
        case gli::TARGET_CUBE:
            glTexImage2D(target,
                         0,
                         format.Internal,
                         props.size.x,
                         props.size.y,
                         0,
                         format.External,
                         GL_FLOAT,
                         nullptr);
            break;
        case gli::TARGET_2D_ARRAY:
        case gli::TARGET_3D:
        case gli::TARGET_CUBE_ARRAY:
            glTexImage3D(target,
                         0,
                         format.Internal,
                         props.size.x,
                         props.size.y,
                         props.size.z,
                         0,
                         format.External,
                         GL_FLOAT,
                         nullptr);
            break;
        default:
            Assertion(false, "Unknown texture target encountered!");
            break;
    }

    _format = props.format;
    _swizzles = gli::swizzles(gli::SWIZZLE_RED,
                              gli::SWIZZLE_GREEN,
                              gli::SWIZZLE_BLUE,
                              gli::SWIZZLE_ALPHA);
    _extent = props.size;
    GLState->Texture.bindTexture(0, _target, 0);
}
void GL3Texture::setFilterProperties(const FilterProperties& props) const {
    glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, convertFilterMode(props.magnification_filter));
    glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, convertFilterMode(props.minification_filter));
    glTexParameteri(_target, GL_TEXTURE_WRAP_S, convertWrapMode(props.wrap_behavior_s));
    glTexParameteri(_target, GL_TEXTURE_WRAP_T, convertWrapMode(props.wrap_behavior_t));
    glTexParameteri(_target, GL_TEXTURE_WRAP_R, convertWrapMode(props.wrap_behavior_r));
    glTexParameterfv(_target, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(props.border_color));
}
void GL3Texture::initialize(const gli::texture& texture, const FilterProperties& filterProperties) {
    gli::gl GL(gli::gl::PROFILE_GL33);
    gli::gl::format const format = GL.translate(texture.format(), texture.swizzles());
    GLenum target = GL.translate(texture.target());

    reset(target, _handle);
    bind(0);

    glTexParameteri(_target, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(_target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(texture.levels() - 1));
    glTexParameteri(_target, GL_TEXTURE_SWIZZLE_R, format.Swizzles[0]);
    glTexParameteri(_target, GL_TEXTURE_SWIZZLE_G, format.Swizzles[1]);
    glTexParameteri(_target, GL_TEXTURE_SWIZZLE_B, format.Swizzles[2]);
    glTexParameteri(_target, GL_TEXTURE_SWIZZLE_A, format.Swizzles[3]);

    setFilterProperties(filterProperties);

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
                             (GLsizei) (texture.target() == gli::TARGET_2D ? size.y : texture.faces()),
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
                             (GLsizei) (texture.target() == gli::TARGET_3D ? size.z : texture.faces()),
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
    _swizzles = texture.swizzles();
    GLState->Texture.bindTexture(0, _target, 0);
}
void GL3Texture::update(const gli::extent3d& position,
                        const gli::extent3d& size,
                        const gli::format dataFormat,
                        const void* data) {
    Assertion(!gli::is_compressed(_format), "Only not compressed textures can be updated!");

    gli::gl GL(gli::gl::PROFILE_GL33);
    gli::gl::format const glDataFormat = GL.translate(dataFormat, gli::swizzles(gli::SWIZZLE_RED,
                                                                                gli::SWIZZLE_GREEN,
                                                                                gli::SWIZZLE_BLUE,
                                                                                gli::SWIZZLE_ALPHA));

    bind(0);
    switch (_target) {
        case GL_TEXTURE_1D:
            glTexSubImage1D(
                _target, 0, position.x, size.x, glDataFormat.External, glDataFormat.Type, data);
            break;
        case GL_TEXTURE_1D_ARRAY:
        case GL_TEXTURE_2D:
        case GL_TEXTURE_CUBE_MAP:
            glTexSubImage2D(
                _target, 0,
                position.x, position.y,
                size.x, size.y,
                glDataFormat.External, glDataFormat.Type, data);
            break;
        case GL_TEXTURE_2D_ARRAY:
        case GL_TEXTURE_3D:
            glTexSubImage3D(
                _target, 0,
                position.x, position.y, position.z,
                size.x, size.y, size.z,
                glDataFormat.External, glDataFormat.Type, data);
            break;
        default:
            Assertion(false, "Unknown texture target encountered!");
            break;
    }
    GLState->Texture.bindTexture(0, _target, 0);
}
gli::extent3d GL3Texture::getSize() const {
    return _extent;
}


