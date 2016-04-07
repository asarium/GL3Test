#include "GL3State.hpp"

thread_local std::unique_ptr<GL3StateTracker> GLState;

void GL3BufferState::bindArrayBuffer(GLuint buffer) {
    _arrayBuffer.setWanted(buffer);
}

void GL3BufferState::bindElementBuffer(GLuint buffer) {
    _elementBuffer.setWanted(buffer);
}

void GL3BufferState::flushStateChanges(bool reset) {
    _arrayBuffer.apply(reset);
    _elementBuffer.apply(reset);
}


GL3TextureState::GL3TextureState() {
    GLint units;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &units);
    _textureUnits.resize(static_cast<size_t>(units));
}

void GL3TextureState::setActiveUnit(uint32_t unit) {
    if (_activeTextureUnit.setIfChanged(unit)) {
        glActiveTexture(GL_TEXTURE0 + unit);
    }
}

void GL3TextureState::bindTexture(int tex_unit, GLenum target, GLuint handle) {
    _textureUnits[tex_unit].boundTexture.setWanted(std::make_pair(target, handle));
}

void GL3TextureState::unbindAll() {
    for (size_t i = 0; i < _textureUnits.size(); ++i) {
        _textureUnits[i].boundTexture.reset();
    }
}

void GL3TextureState::flushStateChanges(bool reset) {
    for (size_t i = 0; i < _textureUnits.size(); ++i) {
        if (_textureUnits[i].boundTexture.needsChange()) {
            setActiveUnit((uint32_t) i);

            _textureUnits[i].boundTexture.apply(reset);
        }
    }
}


void GL3StateTracker::setDepthTest(bool enable) {
    _depthTest.setWanted(enable);
}

void GL3StateTracker::setDepthMask(bool flag) {
    _depthMask.setWanted(flag);
}

void GL3StateTracker::setDepthFunc(DepthFunction mode) {
    _depthFunction.setWanted(mode);
}

void GL3StateTracker::bindVertexArray(GLuint handle) {
    _vertexArray.setWanted(handle);
}

void GL3StateTracker::bindRenderBuffer(GLuint renderbuffer) {
    _boundRenderbuffer.setWanted(renderbuffer);
}

void GL3StateTracker::setBlendMode(bool enable) {
    _blendEnabled.setWanted(enable);
}


void GL3StateTracker::setBlendFunc(BlendFunction mode) {
    _blendFunc.setWanted(mode);
}

void GL3StateTracker::flushStateChanges(bool reset) {
    Texture.flushStateChanges(reset);
    Program.flushStateChanges(reset);
    Buffer.flushStateChanges(reset);

    _depthTest.apply(reset);
    _depthMask.apply(reset);
    _depthFunction.apply(reset);
    _vertexArray.apply(reset);
    _boundRenderbuffer.apply(reset);
    _blendEnabled.apply(reset);
    _blendFunc.apply(reset);
}


void GL3ProgramState::use(GLuint handle) {
    _activeProgram.setWanted(handle);
}

void GL3ProgramState::flushStateChanges(bool reset) {
    _activeProgram.apply(reset);
}


GL3FramebufferState::GL3FramebufferState() {
    _activeDrawBuffer = 0;
    _activeReadBuffer = 0;
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

