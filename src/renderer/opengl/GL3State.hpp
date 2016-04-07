#pragma once

#include <glad/glad.h>

#include <memory>
#include <vector>
#include <stack>
#include <renderer/PipelineState.hpp>

#include "GL3StateFunctors.hpp"

template<typename T>
class SavedState {
public:
    typedef T saved_type;

private:
    saved_type _saved;
    bool _dirty; // true if the saved value is not up to date

public:
    SavedState() : _dirty(true) { }

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

template<typename T, typename ApplyFunc>
class DeferredState {
public:
    typedef T saved_type;
    typedef ApplyFunc apply_type;

private:
    saved_type _saved;
    saved_type _wanted;

    apply_type _applyFunc;

    bool _dirty; // true if the saved value is not up to date
public:
    DeferredState() : _dirty(true), _wanted(apply_type::default_value()) { }

    void setWanted(const saved_type &values) {
        _wanted = values;
    }

    void markDirty() {
        _dirty = true;
    }

    void apply(bool reset) {
        if (needsChange()) {
            _applyFunc(_wanted);

            _saved = _wanted;
            _dirty = false;
        }

        if (reset) {
            // Always reset back to default so every code uses the same default OpenGL State
            this->reset();
        }
    }

    bool needsChange() {
        return _dirty || _wanted != _saved;
    }

    void reset() {
        _wanted = apply_type::default_value();
    }

    const saved_type &operator*() {
        return _wanted;
    }
};

class GL3BufferState {
    DeferredState<GLuint, BindArrayBuffer> _arrayBuffer;
    DeferredState<GLuint, BindElementBuffer> _elementBuffer;
public:
    void bindArrayBuffer(GLuint buffer);

    void bindElementBuffer(GLuint buffer);

    void flushStateChanges(bool reset);
};

class GL3TextureState {
    struct GL3TextureUnit {
        DeferredState<std::pair<GLenum, GLuint>, BindTexture> boundTexture;
    };

    SavedState<uint32_t> _activeTextureUnit;
    std::vector<GL3TextureUnit> _textureUnits;

public:
    GL3TextureState();

    void setActiveUnit(uint32_t unit);

    void bindTexture(int tex_unit, GLenum target, GLuint handle);

    void unbindAll();

    void flushStateChanges(bool reset);
};

class GL3ProgramState {
    DeferredState<GLuint, UseProgram> _activeProgram;
public:
    void use(GLuint program);

    void flushStateChanges(bool reset);
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
    DeferredState<bool, SetDepthTest> _depthTest;
    DeferredState<bool, SetDepthMask> _depthMask;
    DeferredState<DepthFunction, SetDepthFunction> _depthFunction;

    DeferredState<GLuint, BindVertexArray> _vertexArray;
    DeferredState<GLuint, BindRenderBuffer> _boundRenderbuffer;

    DeferredState<bool, SetBlendMode> _blendEnabled;
    DeferredState<BlendFunction, SetBlendFunc> _blendFunc;

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

    void flushStateChanges(bool reset = true);
};

extern thread_local std::unique_ptr<GL3StateTracker> GLState;
