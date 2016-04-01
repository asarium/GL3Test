//
//

#include "GL3Texture2D.hpp"
#include "GL3State.hpp"

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

GL3Texture2D::GL3Texture2D() {
    glGenTextures(1, &_textureHandle);
}

GL3Texture2D::~GL3Texture2D() {
    glDeleteTextures(1, &_textureHandle);
}

void GL3Texture2D::bind(int tex_unit) {
    GLState->Texture.bindTexture(tex_unit, GL_TEXTURE_2D, _textureHandle);
}

void GL3Texture2D::initialize(size_t width, size_t height, TextureFormat format, void *data) {
    this->bind(0);

    _props.format = getFormat(format);
    _props.internal_format = getInternalFormat(format);
    _props.width = static_cast<GLsizei>(width);
    _props.height = static_cast<GLsizei>(height);

    glTexImage2D(GL_TEXTURE_2D, 0, _props.internal_format, _props.width, _props.height, 0, _props.format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    GLState->Texture.bindTexture(0, GL_TEXTURE_2D, 0);
}

void GL3Texture2D::updateDate(void *data) {
    this->bind(0);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _props.width, _props.height, _props.format, GL_UNSIGNED_BYTE, data);

    GLState->Texture.bindTexture(0, GL_TEXTURE_2D, 0);
}
