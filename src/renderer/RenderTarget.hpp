#pragma once

#include <glm/glm.hpp>

#include "Texture.hpp"
#include "Util.hpp"

class RenderTarget {
public:
    virtual ~RenderTarget() {}

    virtual size_t getWidth() const = 0;

    virtual size_t getHeight() const = 0;

    virtual void copyToTexture(PointerWrapper<Texture> target) = 0;

    virtual std::vector<TextureHandle*> getColorTextures() = 0;

    virtual TextureHandle* getDepthTexture() = 0;
};


