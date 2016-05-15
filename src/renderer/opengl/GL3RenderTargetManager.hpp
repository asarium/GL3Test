#pragma once

#include "renderer/RenderTargetManager.hpp"
#include "GL3RenderTarget.hpp"
#include "GL3Object.hpp"

class GL3RenderTargetManager final: GL3Object, public RenderTargetManager {
    GL3RenderTarget* _currentRenderTarget;

    std::unique_ptr<GL3RenderTarget> _defaultRenderTarget;

    std::stack<GL3RenderTarget*> _renderTargetStack;
 public:
    GL3RenderTargetManager(GL3Renderer* renderer);
    virtual ~GL3RenderTargetManager() { };

    void updateDefaultTarget(uint32_t width, uint32_t height);

    virtual std::unique_ptr<RenderTarget> createRenderTarget(const RenderTargetProperties& properties) override;

    virtual void useRenderTarget(RenderTarget* target) override;

    virtual RenderTarget* getCurrentRenderTarget() override;

    virtual void pushRenderTargetBinding() override;

    virtual void popRenderTargetBinding() override;
};


