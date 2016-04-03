#pragma once

#include <cstddef>

enum class TextureFormat {
    R8G8B8A8,
    R8G8B8
};

class Texture2D {
public:
    virtual ~Texture2D() { }

    virtual void initialize(size_t width, size_t height, TextureFormat format, void* data) = 0;

    virtual void updateData(void *data) = 0;
};
