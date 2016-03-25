#pragma once

#include "ShaderProgram.hpp"
#include "VertexLayout.hpp"
#include "ShaderParameters.hpp"

class DrawCall {
public:
    virtual ~DrawCall() { }

    virtual void draw() = 0;

    virtual ShaderParameters* getParameters() = 0;
};
