#pragma once

#include "RenderTarget.hpp"

#include <memory>

enum class ColorBufferFormat {
    RGB,
    RGBA,
    RGB16F,
    RGBA16F,
    RGB32F,
    RGBA32F
};

struct RenderTargetProperties {
    size_t width;
    size_t height;
    std::vector<ColorBufferFormat> color_buffers;

    struct {
        bool enable;
        bool make_texture_handle;
    } depth;
};

class RenderTargetManager {
 public:
    virtual ~RenderTargetManager() { }

    virtual std::unique_ptr<RenderTarget> createRenderTarget(const RenderTargetProperties& properties) = 0;

    virtual void useRenderTarget(RenderTarget* target) = 0;

    virtual RenderTarget* getCurrentRenderTarget() = 0;

    virtual void pushRenderTargetBinding() = 0;

    virtual void popRenderTargetBinding() = 0;
};

