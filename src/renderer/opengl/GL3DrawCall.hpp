#pragma once

#include "renderer/DrawCall.hpp"
#include "GL3ShaderProgram.hpp"
#include "GL3VertexLayout.hpp"
#include "GL3DrawCallManager.hpp"
#include "GL3Util.hpp"


class GL3DrawCall final: public GL3Object, public DrawCall, public VariableDrawCall,
                         public InstancedDrawCall, public InstancedVariableDrawCall {
    GL3DrawCallProperties _properties;

    GL3ShaderParameters _parameters;

    VariableStackArray<> _pushConstants;

    void setGLState();

    void actualDraw(GLsizei count, GLint offset);

    void actualDrawInstanced(GLsizei instances, GLsizei count, GLint offset);

 public:
    GL3DrawCall(GL3Renderer* renderer, const GL3DrawCallProperties&);

    ~GL3DrawCall();

    virtual void draw() override;

    virtual void draw(size_t count, size_t offset) override;

    virtual void drawInstanced(size_t num_instances) override;

    virtual void drawInstanced(size_t num_instances, size_t count, size_t offset) override;

    virtual ShaderParameters* getParameters() override;

    virtual void setPushConstants(const void* data, size_t size) override;
};


