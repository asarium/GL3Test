//
//

#include "GL3Texture2D.hpp"

namespace {
    GLenum getInternalFormat(TextureFormat format) {
        switch (format) {
            case TextureFormat::R8G8B8A8:
                return GL_RGBA;
            case TextureFormat::R8G8B8:
                return GL_RGB;
        }
    }

    GLenum getFormat(TextureFormat format) {
        switch (format) {
            case TextureFormat::R8G8B8A8:
                return GL_RGBA;
            case TextureFormat::R8G8B8:
                return GL_RGB;
        }
    }
}

GL3Texture2D::GL3Texture2D() {
    glGenTextures(1, &_textureHandle);
}

GL3Texture2D::~GL3Texture2D() {
    glDeleteTextures(1, &_textureHandle);
}

void GL3Texture2D::bind() {
    glBindTexture(GL_TEXTURE_2D, _textureHandle);
}

void GL3Texture2D::initialize(size_t width, size_t height, TextureFormat format, void *data) {
    this->bind();

    _props.format = getFormat(format);
    _props.internal_format = getInternalFormat(format);
    _props.width = static_cast<GLsizei>(width);
    _props.height = static_cast<GLsizei>(height);

    glTexImage2D(GL_TEXTURE_2D, 0, _props.internal_format, _props.width, _props.height, 0, _props.format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void GL3Texture2D::updateDate(void *data) {
    this->bind();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _props.width, _props.height, _props.format, GL_UNSIGNED_BYTE, data);
}
