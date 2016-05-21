#pragma once

#include "renderer/DrawCall.hpp"
#include "GL3VertexLayout.hpp"
#include "GL3DrawCallManager.hpp"
#include "GL3Util.hpp"


class GL3DrawCall final: public GL3Object, public DrawCall, public VariableDrawCall,
                         public InstancedDrawCall, public InstancedVariableDrawCall {
    GL3DrawCallProperties _properties;

    VariableStackArray<> _pushConstants;

    void setGLState();

    void actualDraw(GLint offset, GLsizei count);

    void actualDrawInstanced(GLsizei instances, GLsizei offset, GLint count);

 public:
    GL3DrawCall(GL3Renderer* renderer, const GL3DrawCallProperties&);

    ~GL3DrawCall();

    virtual void draw() override;

    virtual void draw(size_t offset, size_t count) override;

    virtual void drawInstanced(size_t num_instances) override;

    virtual void drawInstanced(size_t num_instances, size_t offset, size_t count) override;

    virtual void setPushConstants(const void* data, size_t size) override;
};


