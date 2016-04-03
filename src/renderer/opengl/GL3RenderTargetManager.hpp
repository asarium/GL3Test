#pragma once

#include "renderer/RenderTargetManager.hpp"
#include "GL3RenderTarget.hpp"
#include "GL3Object.hpp"

class GL3RenderTargetManager : GL3Object, public RenderTargetManager {
    GL3RenderTarget* _currentRenderTarget;

    std::unique_ptr<GL3RenderTarget> _defaultRenderTarget;
public:
    GL3RenderTargetManager(GL3Renderer* renderer);
    virtual ~GL3RenderTargetManager() {};

    void initialize(int width, int height);

    GL3RenderTarget* getCurrentRenderTarget();

    virtual std::unique_ptr<RenderTarget> createRenderTarget(size_t width, size_t height) override;

    virtual void useRenderTarget(RenderTarget *target) override;
};


