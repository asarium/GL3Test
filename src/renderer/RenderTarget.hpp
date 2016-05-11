#pragma once

#include <glm/glm.hpp>

#include "Texture.hpp"

class RenderTarget {
public:
    virtual ~RenderTarget() {}

    virtual size_t getWidth() const = 0;

    virtual size_t getHeight() const = 0;

    virtual void copyToTexture(Texture* target) = 0;

    virtual TextureHandle* getColorTexture() = 0;

    virtual TextureHandle* getDepthTexture() = 0;
};


