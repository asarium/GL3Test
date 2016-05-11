#pragma once

#include <cstddef>
#include <gli/texture.hpp>

class Texture2DHandle {
protected:
    Texture2DHandle() {}
public:
    virtual ~Texture2DHandle() {}

    virtual int getNanoVGHandle() = 0;
};

class Texture2D : public Texture2DHandle {
public:
    virtual ~Texture2D() { }
    virtual void initialize(const gli::texture& texture) = 0;
};
