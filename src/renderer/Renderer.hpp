#ifndef PROJECT_RENDERER_H
#define PROJECT_RENDERER_H
#pragma once

#include <SDL_video.h>

#include <glm/glm.hpp>

#include <memory>

#include "BufferObject.hpp"
#include "VertexLayout.hpp"
#include "util/FileLoader.hpp"
#include "DrawCall.hpp"
#include "DrawCallManager.hpp"
#include "ShaderParameters.hpp"
#include "LightingManager.hpp"
#include "RenderTargetManager.hpp"
#include "Exceptions.hpp"
#include "Profiler.hpp"

#include "nanovg/nanovg.h"

enum class SettingsLevel {
    Disabled,
    Low,
    Medium,
    High,
    Ultra
};

enum class SettingsParameterType {
    Boolean
};

enum class SettingsParameter {
    Resolution,
    VerticalSync,
    MultiSamplingAntiAliasing,
    Shadows
};

struct RendererSettings {
    glm::uvec2 resolution;
    bool vertical_sync;
    uint32_t msaa_samples;
    SettingsLevel shadow_quality;
};

class RendererSettingsManager {
 public:
    virtual ~RendererSettingsManager() { }

    virtual void changeSettings(const RendererSettings& settings) = 0;

    virtual RendererSettings getCurrentSettings() const = 0;

    virtual bool supportsSetting(SettingsParameter parameter) const = 0;
};

enum class GraphicsCapability {
    PointSprites,
    S3TC,
    Instancing,
    FloatingPointTextures
};

class Renderer {
 public:
    virtual ~Renderer() { }

    virtual SDL_Window* initialize() = 0;

    virtual RendererSettingsManager* getSettingsManager() = 0;

    virtual DrawCallManager* getDrawCallManager() = 0;

    virtual LightingManager* getLightingManager() = 0;

    virtual RenderTargetManager* getRenderTargetManager() = 0;

    virtual Profiler* getProfiler() = 0;

    virtual std::unique_ptr<BufferObject> createBuffer(BufferType type) = 0;

    virtual std::unique_ptr<VertexLayout> createVertexLayout() = 0;

    virtual std::unique_ptr<Texture2D> createTexture() = 0;

    virtual std::unique_ptr<PipelineState> createPipelineState(const PipelineProperties& props) = 0;

    virtual std::unique_ptr<DescriptorSet> createDescriptorSet(DescriptorSetType type) = 0;

    virtual bool hasCapability(GraphicsCapability capability) const = 0;

    virtual void clear(const glm::vec4& color) = 0;

    virtual void presentNextFrame() = 0;

    virtual void deinitialize() = 0;

    virtual NVGcontext* getNanovgContext() = 0;

    virtual void nanovgEndFrame() = 0;
};

#endif //PROJECT_RENDERER_H
