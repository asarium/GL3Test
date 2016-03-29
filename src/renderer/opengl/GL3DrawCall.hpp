#pragma once

#include "renderer/DrawCall.hpp"
#include "GL3ShaderProgram.hpp"
#include "GL3VertexLayout.hpp"
#include "GL3DrawCallManager.hpp"


class GL3DrawCall : public DrawCall {
    GL3DrawCallProperties _properties;

    GL3ShaderParameters _parameters;

    void setGLState();
public:
    GL3DrawCall(const GL3DrawCallProperties &);

    ~GL3DrawCall();

    virtual void draw() override;

    virtual ShaderParameters* getParameters() override;
};


