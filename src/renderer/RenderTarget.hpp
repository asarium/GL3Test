#pragma once

#include <glm/glm.hpp>

class RenderTarget {
public:
    virtual ~RenderTarget() {}

    virtual size_t getWidth() = 0;

    virtual size_t getHeight() = 0;
};


