#pragma once

#include "RenderTarget.hpp"

#include <memory>

class RenderTargetManager {
public:
    virtual ~RenderTargetManager() { }

    virtual std::unique_ptr<RenderTarget> createRenderTarget(size_t width, size_t height) = 0;

    virtual void useRenderTarget(RenderTarget* target) = 0;
};

