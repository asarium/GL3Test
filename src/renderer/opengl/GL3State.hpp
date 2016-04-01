#pragma once

#include <glad/glad.h>

#include <memory>
#include <vector>
#include <renderer/PipelineState.hpp>

template<typename T, T DefaultValue = 0>
class SavedState {
public:
    typedef T saved_type;

private:
    saved_type _saved;
    bool _dirty; // true if the saved value is not up to date

public:
    SavedState() : _saved(DefaultValue), _dirty(true) { }

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
        return _dirty ? DefaultValue : _saved;
    }
};

class GL3BufferState {
    SavedState<GLuint> _arrayBuffer;
    SavedState<GLuint> _elementBuffer;
public:
    void bindArrayBuffer(GLuint buffer);

    void bindElementBuffer(GLuint buffer);
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

class GL3StateTracker {
    SavedState<bool, false> _depthTest;
    SavedState<GLuint> _vertexArray;
    SavedState<GLuint> _boundFramebuffer;
    SavedState<GLuint> _boundRenderbuffer;

    SavedState<bool> _blendEnabled;
    SavedState<BlendFunction, BlendFunction::Additive> _blendFunc;

public:
    GL3BufferState Buffer;
    GL3TextureState Texture;
    GL3ProgramState Program;

    void setDepthTest(bool enable);

    void bindVertexArray(GLuint handle);

    void bindFramebuffer(GLuint framebuffer);

    void bindRenderBuffer(GLuint renderbuffer);

    void setBlendMode(bool enable);
    void setBlendFunc(BlendFunction mode);
};

extern thread_local std::unique_ptr<GL3StateTracker> GLState;
