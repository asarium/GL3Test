#pragma once

#include <glad/glad.h>

struct BindArrayBuffer {
    static constexpr GLuint default_value() {
        return 0;
    }

    void operator()(GLuint id) {
        glBindBuffer(GL_ARRAY_BUFFER, id);
    }
};

struct BindElementBuffer {
    static constexpr GLuint default_value() {
        return 0;
    }

    void operator()(GLuint id) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    }
};

struct BindTexture {
    static constexpr std::pair<GLenum, GLuint> default_value() {
        return std::make_pair(GL_TEXTURE_2D, 0);
    }

    void operator()(const std::pair<uint32_t, uint32_t> &vals) {
        glBindTexture(vals.first, vals.second);
    }
};

struct SetActiveTextureUnit {
    static constexpr GLuint default_value() {
        return 0;
    }

    void operator()(uint32_t tex_unit) {
        glActiveTexture(GL_TEXTURE0 + tex_unit);
    }
};

struct UseProgram {
    static constexpr GLuint default_value() {
        return 0;
    }

    void operator()(GLuint program) {
        glUseProgram(program);
    }
};

struct BindFramebuffer {
    static constexpr std::pair<GLuint, GLuint> default_value() {
        return std::make_pair(0, 0);
    }

    void operator()(const std::pair<GLuint, GLuint> &buffers) {
        if (buffers.first == buffers.second) {
            glBindFramebuffer(GL_FRAMEBUFFER, buffers.first);
        } else {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, buffers.first);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buffers.second);
        }
    }
};

struct SetDepthTest {
    static constexpr bool default_value() {
        return true;
    }

    void operator()(bool enable) {
        if (enable) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }
    }
};

struct SetDepthMask {
    static constexpr bool default_value() {
        return true;
    }

    void operator()(bool enable) {
        glDepthMask(enable ? GL_TRUE : GL_FALSE);
    }
};

struct SetDepthFunction {
    static constexpr DepthFunction default_value() {
        return DepthFunction::Less;
    }

    void operator()(DepthFunction mode) {
        GLenum depthMode;

        switch (mode) {
            case DepthFunction::Always:
                depthMode = GL_ALWAYS;
                break;
            case DepthFunction::Equal:
                depthMode = GL_EQUAL;
                break;
            case DepthFunction::Greater:
                depthMode = GL_GREATER;
                break;
            case DepthFunction::GreaterOrEqual:
                depthMode = GL_GEQUAL;
                break;
            case DepthFunction::Less:
                depthMode = GL_LESS;
                break;
            case DepthFunction::LessOrEqual:
                depthMode = GL_LEQUAL;
                break;
            case DepthFunction::Never:
                depthMode = GL_NEVER;
                break;
            case DepthFunction::NotEqual:
                depthMode = GL_NOTEQUAL;
                break;
        }

        glDepthFunc(depthMode);
    }
};

struct BindVertexArray {
    static constexpr GLuint default_value() {
        return 0;
    }

    void operator()(GLuint handle) {
        glBindVertexArray(handle);
    }
};

struct BindRenderBuffer {
    static constexpr GLuint default_value() {
        return 0;
    }

    void operator()(GLuint handle) {
        glBindRenderbuffer(GL_RENDERBUFFER, handle);
    }
};

struct SetBlendMode {
    static constexpr bool default_value() {
        return false;
    }

    void operator()(bool enable) {
        if (enable) {
            glEnable(GL_BLEND);
        } else {
            glDisable(GL_BLEND);
        }
    }
};

struct SetBlendFunc {
    static constexpr BlendFunction default_value() {
        return BlendFunction::None;
    }

    void operator()(BlendFunction mode) {
        switch (mode) {
            case BlendFunction::None:
                break;
            case BlendFunction::Additive:
                glBlendFunc(GL_ONE, GL_ONE);
                break;
            case BlendFunction::AdditiveAlpha:
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                break;
        }
    }
};

