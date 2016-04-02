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

void GL3StateTracker::setDepthFunc(DepthFunction mode) {
	if (_depthFunction.setIfChanged(mode)) {
		GLint depthMode;

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
}

void GL3StateTracker::bindVertexArray(GLuint handle) {
    if (_vertexArray.setIfChanged(handle)) {
        glBindVertexArray(handle);
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
        switch (mode) {
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


void GL3FramebufferState::bindRead(GLuint name) {
    if (_activeReadBuffer.setIfChanged(name)) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, name);
    }
}

void GL3FramebufferState::bindDraw(GLuint name) {
    if (_activeDrawBuffer.setIfChanged(name)) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, name);
    }
}

void GL3FramebufferState::bind(GLuint name) {
    // GL_FRAMEBUFFER is a shortcut for both GL_DRAW_FRAMEBUFFER and GL_READ_FRAMEBUFFER
    if (_activeDrawBuffer.isNewValue(name) || _activeReadBuffer.isNewValue(name)) {
        _activeReadBuffer = name;
        _activeDrawBuffer = name;

        glBindFramebuffer(GL_FRAMEBUFFER, name);
    }
}

void GL3FramebufferState::pushBinding() {
    FramebufferBinding binding;
    binding.drawBuffer = *_activeDrawBuffer;
    binding.readBuffer = *_activeReadBuffer;

    _framebufferStack.push(binding);
}

void GL3FramebufferState::popBinding() {
    auto previousBinding = _framebufferStack.top();
    _framebufferStack.pop();

    // Restore the previous state
    if (previousBinding.readBuffer == previousBinding.drawBuffer) {
        bind(previousBinding.drawBuffer);
    } else {
        bindRead(previousBinding.readBuffer);
        bindDraw(previousBinding.drawBuffer);
    }
}

