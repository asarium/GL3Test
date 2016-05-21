#pragma once

#include <glad/glad.h>

#include <memory>
#include <vector>
#include <stack>
#include <renderer/PipelineState.hpp>

template<typename T>
class SavedState {
public:
    typedef T saved_type;

private:
    bool _dirty; // true if the saved value is not up to date
    saved_type _saved;

public:
    SavedState() : _dirty(true), _saved(T()) { }

    bool setIfChanged(saved_type new_val) {
        if (_dirty || new_val != _saved) {
            _saved = new_val;
            _dirty = false;
            return true;
        }
        return false;
    }

    bool isNewValue(saved_type newVal) {
        return _dirty || newVal != _saved;
    }

    void markDirty() {
        _dirty = true;
    }

    bool isDirty() {
        return _dirty;
    }

    saved_type operator*() {
        return _saved;
    }

    SavedState &operator=(const saved_type &value) {
        setIfChanged(value);
        return *this;
    }
};

class GL3BufferState {
    SavedState<GLuint> _arrayBuffer;
    SavedState<GLuint> _elementBuffer;
    SavedState<GLuint> _uniformBuffer;
public:
    void bindArrayBuffer(GLuint buffer);

    void bindElementBuffer(GLuint buffer);

    void bindUniformBuffer(GLuint buffer);
};

class GL3TextureState {
    struct GL3TextureUnit {
        SavedState<GLuint> boundTexture;
        SavedState<GLenum> textureTarget;
    };

    SavedState<int> _activeTextureUnit;
    std::vector<GL3TextureUnit> _textureUnits;

public:
    GL3TextureState();

    void setActiveUnit(int tex_unit);

    void bindTexture(GLenum target, GLuint handle);

    void bindTexture(int tex_unit, GLenum target, GLuint handle);

    void unbindAll();
};

class GL3ProgramState {
    SavedState<GLuint> _activeProgram;
public:
    void use(GLuint program);
};

class GL3FramebufferState {
    struct FramebufferBinding {
        GLuint readBuffer;
        GLuint drawBuffer;
    };

    SavedState<GLuint> _activeReadBuffer;
    SavedState<GLuint> _activeDrawBuffer;

    std::stack<FramebufferBinding> _framebufferStack;
public:
    GL3FramebufferState();

    void bindRead(GLuint name);

    void bindDraw(GLuint name);

    void bind(GLuint name);

    void pushBinding();

    void popBinding();
};

class Gl3ContextConstants
{
    GLint _uniformBufferAlignment;

public:
    Gl3ContextConstants();

    GLint getUniformBufferAlignment() const
    {
        return _uniformBufferAlignment;
    }
};

class GL3Stencil
{
    SavedState<bool> _stencilEnabled;
    SavedState<GLuint> _stencilMask;
    SavedState<std::tuple<GLenum, uint32_t, uint32_t>> _stencilFunc;

    SavedState<std::tuple<GLenum, GLenum, GLenum>> _frontStencilOp;
    SavedState<std::tuple<GLenum, GLenum, GLenum>> _backStencilOp;

public:
    void setStencilTest(bool enable);

    void setStencilMask(GLuint mask);

    void setFrontStencilOp(const std::tuple<GLenum, GLenum, GLenum>& values);
    void setBackStencilOp(const std::tuple<GLenum, GLenum, GLenum>& values);

    void setStencilFunc(const std::tuple<GLenum, uint32_t, uint32_t>& values);
};

class GL3CullFace{
    SavedState<bool> _faceCulling;
    SavedState<GLenum> _culledFace;
    SavedState<GLenum> _frontFace;

public:
    void setFaceCulling(bool culling);

    void setCulledFace(GLenum culledFace);

    void setFrontFace(GLenum frontFace);
};

class GL3StateTracker {
    SavedState<bool> _depthTest;
    SavedState<bool> _depthMask;
    SavedState<ComparisionFunction> _depthFunction;

    SavedState<GLuint> _vertexArray;
    SavedState<GLuint> _boundRenderbuffer;

    SavedState<bool> _blendEnabled;
    SavedState<BlendFunction> _blendFunc;

    SavedState<bool> _scissorTest;

    SavedState<glm::bvec4> _colorMask;
public:
    GL3BufferState Buffer;
    GL3TextureState Texture;
    GL3ProgramState Program;
    GL3FramebufferState Framebuffer;
    Gl3ContextConstants Constants;
    GL3Stencil Stencil;
    GL3CullFace CullFace;

    void setDepthTest(bool enable);

    void setDepthMask(bool flag);

    void setDepthFunc(ComparisionFunction mode);

    void bindVertexArray(GLuint handle);

    void bindRenderBuffer(GLuint renderbuffer);

    void setBlendMode(bool enable);

    void setBlendFunc(BlendFunction mode);

    void setScissorTest(bool enable);

    void setColorMask(const glm::bvec4& mask);
};

extern thread_local std::unique_ptr<GL3StateTracker> GLState;
