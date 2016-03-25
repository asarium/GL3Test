#pragma once

#include "ShaderProgram.hpp"
#include "VertexLayout.hpp"

class DrawCall {
public:
    virtual ~DrawCall() { }

    virtual void draw() = 0;
};
