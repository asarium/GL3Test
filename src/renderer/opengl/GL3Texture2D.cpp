//
//

#include "GL3Texture2D.hpp"
#include "GL3State.hpp"
#include "GL3Renderer.hpp"

#include <renderer/Exceptions.hpp>

namespace {
GLenum getInternalFormat(TextureFormat format) {
    switch (format) {
        case TextureFormat::R8G8B8A8:
            return GL_RGBA;
        case TextureFormat::R8G8B8:
            return GL_RGB;
    }
    return GL_RGBA;
}

GLenum getCompressionFormat(CompressionFormat format) {
    switch (format) {
        case CompressionFormat::S3TC_RGB_DXT1:
            return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
        case CompressionFormat::S3TC_RGBA_DXT1:
            return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        case CompressionFormat::S3TC_RGBA_DXT3:
            return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        case CompressionFormat::S3TC_RGBA_DXT5:
            return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    }
    return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
}

GLenum getFormat(TextureFormat format) {
    switch (format) {
        case TextureFormat::R8G8B8A8:
            return GL_RGBA;
        case TextureFormat::R8G8B8:
            return GL_RGB;
    }
    return 0;
}
}

GL3Texture2D::GL3Texture2D(GL3Renderer* renderer, GLuint handle) : GL3Object(renderer), _handle(handle), _nvgHandle(0) {
}

GL3Texture2D::GL3Texture2D(GL3Renderer* renderer) : GL3Object(renderer), _handle(), _nvgHandle(0) {
}

GL3Texture2D::~GL3Texture2D() {
}

GLuint GL3Texture2D::getHandle() {
    return *_handle;
}

void GL3Texture2D::bind(int tex_unit) {
    GLState->Texture.bindTexture(tex_unit, GL_TEXTURE_2D, getHandle());
}

void GL3Texture2D::initialize(size_t width, size_t height, TextureFormat format, void* data) {
    this->bind(0);

    _props.internal_format = getInternalFormat(format);
    _props.format = getFormat(format);
    _props.width = static_cast<GLsizei>(width);
    _props.height = static_cast<GLsizei>(height);
    _props.is_compressed = false;

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 _props.internal_format,
                 _props.width,
                 _props.height,
                 0,
                 _props.format,
                 GL_UNSIGNED_BYTE,
                 data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    GLState->Texture.bindTexture(0, GL_TEXTURE_2D, 0);
}

void GL3Texture2D::updateData(void* data) {
    this->bind(0);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _props.width, _props.height, _props.format, GL_UNSIGNED_BYTE, data);

    GLState->Texture.bindTexture(0, GL_TEXTURE_2D, 0);
}

void GL3Texture2D::initializeCompressed(size_t width,
                                        size_t height,
                                        CompressionFormat format,
                                        size_t data_size,
                                        void* data) {
    if (!GLAD_GL_EXT_texture_compression_s3tc) {
        throw RendererException("S3TC is not supported by this driver!");
    }

    this->bind(0);

    _props.internal_format = getCompressionFormat(format);
    _props.is_compressed = true;
    _props.width = static_cast<GLsizei>(width);
    _props.height = static_cast<GLsizei>(height);

    glCompressedTexImage2D(GL_TEXTURE_2D,
                           0,
                           _props.internal_format,
                           _props.width,
                           _props.height,
                           0,
                           (GLsizei) data_size,
                           data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    GLState->Texture.bindTexture(0, GL_TEXTURE_2D, 0);
}

void GL3Texture2D::updateCompressedData(size_t data_size, void* data) {
    if (!GLAD_GL_EXT_texture_compression_s3tc) {
        throw RendererException("S3TC is not supported by this driver!");
    }

    this->bind(0);

    glCompressedTexSubImage2D(GL_TEXTURE_2D,
                              0,
                              0,
                              0,
                              _props.width,
                              _props.height,
                              _props.internal_format,
                              (GLsizei) data_size,
                              data);

    GLState->Texture.bindTexture(0, GL_TEXTURE_2D, 0);
}

void GL3Texture2D::copyDataFromFramebuffer(GLsizei width, GLsizei height) {
    this->bind();

    _props.format = GL_RGBA;
    _props.internal_format = GL_RGBA8;
    _props.width = static_cast<GLsizei>(width);
    _props.height = static_cast<GLsizei>(height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0, width, height, 0);
    GLState->Texture.bindTexture(0, GL_TEXTURE_2D, 0);
}
void GL3Texture2D::reset(GLuint handle) {
    _handle.reset(handle);
}
std::unique_ptr<GL3Texture2D> GL3Texture2D::createTexture(GL3Renderer* renderer) {
    GLuint name;
    glGenTextures(1, &name);
    return std::unique_ptr<GL3Texture2D>(new GL3Texture2D(renderer, name));
}

void GL3Texture2D::updateSize(GLsizei width, GLsizei height) {
    _props.width = width;
    _props.height = height;
}
int GL3Texture2D::getNanoVGHandle() {
    Assertion(*_handle != 0, "Trying to get NanoVG handle from invalid texture!");

    if (_nvgHandle > 0) {
        return _nvgHandle;
    }

    _nvgHandle = _renderer->getNanoVGImageHandle(*_handle, _props.width, _props.height);
    return _nvgHandle;
}



