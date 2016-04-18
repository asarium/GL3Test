#pragma once

#include "RenderTarget.hpp"

#include <memory>

struct RenderTargetProperties
{
    size_t width;
    size_t height;
    bool floating_point;
    bool with_depth_buffer;
};

class RenderTargetManager {
public:
    virtual ~RenderTargetManager() { }

    virtual std::unique_ptr<RenderTarget> createRenderTarget(const RenderTargetProperties& properties) = 0;

    virtual void useRenderTarget(RenderTarget* target) = 0;
};

