#pragma once

#include <glm/glm.hpp>

#include "Texture2D.hpp"

class RenderTarget {
public:
    virtual ~RenderTarget() {}

    virtual size_t getWidth() const = 0;

    virtual size_t getHeight() const = 0;

    virtual void copyToTexture(Texture2D* target) = 0;
};


