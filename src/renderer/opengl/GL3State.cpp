#include "GL3State.hpp"
#include "EnumTranslation.hpp"

namespace
{
    void enableDisableState(GLenum state, bool value)
    {
        if (value)
        {
            glEnable(state);
        } else
        {
            glDisable(state);
        }
    }
}

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

void GL3BufferState::bindUniformBuffer(GLuint buffer) {
    if (_uniformBuffer.setIfChanged(buffer)) {
        glBindBuffer(GL_UNIFORM_BUFFER, buffer);
    }
}

GL3TextureState::GL3TextureState() {
    GLint units;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &units);
    _textureUnits.resize(static_cast<size_t>(units));
}

void GL3TextureState::bindTexture(int tex_unit, GLenum target, GLuint handle) {
    auto& texunit = _textureUnits[tex_unit];

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
        auto& targetState = _textureUnits[i].textureTarget;
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
    auto& texunit = _textureUnits[*_activeTextureUnit];

    if (texunit.textureTarget.isNewValue(target) || texunit.boundTexture.isNewValue(handle)) {
        texunit.textureTarget.setIfChanged(target);
        texunit.boundTexture.setIfChanged(handle);

        glBindTexture(target, handle);
    }
}


void GL3StateTracker::setDepthTest(bool enable) {
    if (_depthTest.setIfChanged(enable)) {
        enableDisableState(GL_DEPTH_TEST, enable);
    }
}

void GL3StateTracker::setDepthMask(bool flag) {
    if (_depthMask.setIfChanged(flag)) {
        glDepthMask(flag ? GL_TRUE : GL_FALSE);
    }
}

void GL3StateTracker::setDepthFunc(ComparisionFunction mode) {
    if (_depthFunction.setIfChanged(mode)) {
        glDepthFunc(convertComparisionFunction(mode));
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
        enableDisableState(GL_BLEND, enable);
    }
}


void GL3StateTracker::setBlendFunc(BlendFunction mode) {
    if (_blendFunc.setIfChanged(mode)) {
        switch (mode) {
            case BlendFunction::None:
                break;
            case BlendFunction::Additive:
                glBlendFunc(GL_ONE, GL_ONE);
                break;
            case BlendFunction::AdditiveAlpha:
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                break;
            case BlendFunction::Alpha:
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                break;
            case BlendFunction::PremultAlpha:
                glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                break;
        }
    }
}

void GL3StateTracker::setScissorTest(bool enable)
{
    if (_scissorTest.setIfChanged(enable))
    {
        enableDisableState(GL_SCISSOR_TEST, enable);
    }
}

void GL3StateTracker::setColorMask(const glm::bvec4& mask)
{
    if (_colorMask.setIfChanged(mask))
    {
        glColorMask(mask[0], mask[1], mask[2], mask[3]);
    }
}

void GL3ProgramState::use(GLuint program) {
    if (_activeProgram.setIfChanged(program)) {
        glUseProgram(program);
    }
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

Gl3ContextConstants::Gl3ContextConstants()
{
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &_uniformBufferAlignment);
}

void GL3Stencil::setStencilTest(bool enable)
{
    if (_stencilEnabled.setIfChanged(enable))
    {
        enableDisableState(GL_STENCIL_TEST, enable);
    }
}

void GL3Stencil::setStencilMask(GLuint mask)
{
    if (_stencilMask.setIfChanged(mask))
    {
        glStencilMask(mask);
    }
}

void GL3Stencil::setFrontStencilOp(const std::tuple<GLenum, GLenum, GLenum>& values)
{
    if (_frontStencilOp.setIfChanged(values))
    {
        glStencilOpSeparate(GL_FRONT, std::get<0>(values),std::get<1>(values),std::get<2>(values));
    }
}
void GL3Stencil::setBackStencilOp(const std::tuple<GLenum, GLenum, GLenum>& values)
{
    if (_backStencilOp.setIfChanged(values))
    {
        glStencilOpSeparate(GL_BACK, std::get<0>(values),std::get<1>(values),std::get<2>(values));
    }
}

void GL3Stencil::setStencilFunc(const std::tuple<GLenum, uint32_t, uint32_t>& values)
{
    if (_stencilFunc.setIfChanged(values))
    {
        glStencilFunc(std::get<0>(values), std::get<1>(values), std::get<2>(values));
    }
}

void GL3CullFace::setFaceCulling(bool culling)
{
    if (_faceCulling.setIfChanged(culling))
    {
        enableDisableState(GL_CULL_FACE, culling);
    }
}

void GL3CullFace::setCulledFace(GLenum culledFace)
{
    if (_culledFace.setIfChanged(culledFace))
    {
        glCullFace(culledFace);
    }
}

void GL3CullFace::setFrontFace(GLenum frontFace)
{
    if (_frontFace.setIfChanged(frontFace))
    {
        glFrontFace(frontFace);
    }
}
