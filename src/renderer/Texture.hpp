#pragma once

#include <cstddef>
#include <gli/texture.hpp>

class TextureHandle {
protected:
    TextureHandle() {}
public:
    virtual ~TextureHandle() {}

    virtual int getNanoVGHandle() = 0;
};

class Texture : public TextureHandle {
public:
    virtual ~Texture() { }
    virtual void initialize(const gli::texture& texture) = 0;
};
