#include "GL3State.hpp"

thread_local std::unique_ptr<GL3StateTracker> GLState;

void GL3BufferState::bindArrayBuffer(GLuint buffer) {
    if (_arrayBuffer.setIfChanged(buffer)) {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
    }
}

void GL3BufferState::bindElementBuffer(GLuint buffer) {
    if (_elementBuffer.setIfChanged(buffer)) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    }
}

GL3TextureState::GL3TextureState() {
    GLint units;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &units);
    _textureUnits.resize(static_cast<size_t>(units));
}

void GL3TextureState::bindTexture(int tex_unit, GLenum target, GLuint handle) {
    auto &texunit = _textureUnits[tex_unit];

    if (texunit.textureTarget.isNewValue(target) || texunit.boundTexture.isNewValue(handle)) {
        texunit.textureTarget.setIfChanged(target);
        texunit.boundTexture.setIfChanged(handle);

        // Only set texture unit if it will actually change something
        setActiveUnit(tex_unit);

        glBindTexture(target, handle);
    }
}

void GL3TextureState::unbindAll() {
    for (size_t i = 0; i < _textureUnits.size(); ++i) {
        auto &targetState = _textureUnits[i].textureTarget;
        auto target = targetState.isDirty() ? GL_TEXTURE_2D : *targetState;
        bindTexture(static_cast<int>(i), target, 0);
    }
}

void GL3TextureState::setActiveUnit(int tex_unit) {
    if (_activeTextureUnit.setIfChanged(tex_unit)) {
        glActiveTexture(GL_TEXTURE0 + tex_unit);
    }
}

void GL3TextureState::bindTexture(GLenum target, GLuint handle) {
    auto &texunit = _textureUnits[*_activeTextureUnit];

    if (texunit.textureTarget.isNewValue(target) || texunit.boundTexture.isNewValue(handle)) {
        texunit.textureTarget.setIfChanged(target);
        texunit.boundTexture.setIfChanged(handle);

        glBindTexture(target, handle);
    }
}


void GL3StateTracker::setDepthTest(bool enable) {
    if (_depthTest.setIfChanged(enable)) {
        if (enable) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }
    }
}

void GL3StateTracker::bindVertexArray(GLuint handle) {
    if (_vertexArray.setIfChanged(handle)) {
        glBindVertexArray(handle);
    }
}

void GL3StateTracker::bindFramebuffer(GLuint framebuffer) {
    if (_boundFramebuffer.setIfChanged(framebuffer)) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    }
}

void GL3StateTracker::bindRenderBuffer(GLuint renderbuffer) {
    if (_boundRenderbuffer.setIfChanged(renderbuffer)) {
        glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    }
}

void GL3StateTracker::setBlendMode(bool enable) {
    if (_blendEnabled.setIfChanged(enable)) {
        if (enable) {
            glEnable(GL_BLEND);
        } else {
            glDisable(GL_BLEND);
        }
    }
}


void GL3StateTracker::setBlendFunc(BlendFunction mode) {
    if (_blendFunc.setIfChanged(mode)) {
        switch(mode) {
            case BlendFunction::Additive:
                glBlendFunc(GL_ONE, GL_ONE);
                break;
        }
    }
}

void GL3ProgramState::use(GLuint handle) {
    if (_activeProgram.setIfChanged(handle)) {
        glUseProgram(handle);
    }
}

