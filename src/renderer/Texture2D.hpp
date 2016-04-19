#pragma once

#include <cstddef>

enum class TextureFormat {
    R8G8B8A8,
    R8G8B8,
};

enum class CompressionFormat {
    S3TC_RGB_DXT1,
    S3TC_RGBA_DXT1,
    S3TC_RGBA_DXT3,
    S3TC_RGBA_DXT5,
};

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

    virtual void initialize(size_t width, size_t height, TextureFormat format, void* data) = 0;

    virtual void updateData(void *data) = 0;

    virtual void initializeCompressed(size_t width, size_t height, CompressionFormat format, size_t data_size, void* data) = 0;

    virtual void updateCompressedData(size_t data_size, void *data) = 0;
};
