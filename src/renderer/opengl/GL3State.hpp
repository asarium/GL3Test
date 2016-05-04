#pragma once

#include <glad/glad.h>

#include <memory>
#include <vector>
#include <stack>
#include <renderer/PipelineState.hpp>
#include "GL3ShaderProgram.hpp"

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
    SavedState<GL3ShaderProgram*> _activeProgram;
public:
    // TODO: Improve this state handling. I don't like using the actual shader program pointer here.
    void use(GL3ShaderProgram* program);

    GL3ShaderProgram* getCurrentProgram() {
        return *_activeProgram;
    }
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

class GL3StateTracker {
    SavedState<bool> _depthTest;
    SavedState<bool> _depthMask;
    SavedState<DepthFunction> _depthFunction;

    SavedState<GLuint> _vertexArray;
    SavedState<GLuint> _boundRenderbuffer;

    SavedState<bool> _blendEnabled;
    SavedState<BlendFunction> _blendFunc;

public:
    GL3BufferState Buffer;
    GL3TextureState Texture;
    GL3ProgramState Program;
    GL3FramebufferState Framebuffer;

    void setDepthTest(bool enable);

    void setDepthMask(bool flag);

    void setDepthFunc(DepthFunction mode);

    void bindVertexArray(GLuint handle);

    void bindRenderBuffer(GLuint renderbuffer);

    void setBlendMode(bool enable);

    void setBlendFunc(BlendFunction mode);
};

extern thread_local std::unique_ptr<GL3StateTracker> GLState;
