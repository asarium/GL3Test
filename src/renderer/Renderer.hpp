#ifndef PROJECT_RENDERER_H
#define PROJECT_RENDERER_H
#pragma once

#include <SDL_video.h>

#include <glm/glm.hpp>

#include <memory>

#include "BufferObject.hpp"
#include "VertexLayout.hpp"
#include "util/FileLoader.hpp"
#include "ShaderParameters.hpp"
#include "RenderTargetManager.hpp"
#include "Exceptions.hpp"
#include "Profiler.hpp"
#include "CommandBuffer.hpp"
#include "PipelineState.hpp"

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
    MultiSamplingAntiAliasing
};

struct RendererSettings {
    glm::uvec2 resolution;
    bool vertical_sync;
    uint32_t msaa_samples;
};

class RendererSettingsManager {
 public:
    virtual ~RendererSettingsManager() {}

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

struct RendererLimits {
    size_t uniform_offset_alignment;
};

class Renderer {
 public:
    virtual ~Renderer() {}

    virtual void initialize(SDL_Window* window) = 0;

    virtual RendererSettingsManager* getSettingsManager() = 0;

    virtual RenderTargetManager* getRenderTargetManager() = 0;

    virtual Profiler* getProfiler() = 0;

    virtual std::unique_ptr<BufferObject> createBuffer(BufferType type) = 0;

    virtual std::unique_ptr<Texture> createTexture() = 0;

    virtual std::unique_ptr<PipelineState> createPipelineState(const PipelineProperties& props) = 0;

    virtual std::unique_ptr<DescriptorSet> createDescriptorSet(DescriptorSetType type) = 0;

    virtual std::unique_ptr<CommandBuffer> createCommandBuffer() = 0;

    virtual std::unique_ptr<VertexArrayObject> createVertexArrayObject(const VertexInputStateProperties& input,
                                                                       const VertexArrayProperties& props) = 0;

    virtual bool hasCapability(GraphicsCapability capability) const = 0;

    virtual RendererLimits getLimits() const = 0;

    virtual void presentNextFrame() = 0;

    virtual void deinitialize() = 0;
};

#endif //PROJECT_RENDERER_H
