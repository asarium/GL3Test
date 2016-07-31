#pragma once

#include <renderer/Renderer.hpp>
#include <util/UniformAligner.hpp>
#include "DrawUtil.hpp"
#include "Light.hpp"

namespace lighting {
class LightingManager {
    Renderer* _renderer;

    DrawUtil _util;

    std::unique_ptr<DrawMesh> _sphereMesh;
    std::unique_ptr<DrawMesh> _fullscreenTriMesh;

    std::unique_ptr<RenderTarget> _lightingRenderTarget;

    std::unique_ptr<BufferObject> _uniformBuffer;

    UniformAligner<LightParameters> _alignedUniformData;

    std::unique_ptr<DescriptorSet> _lightingDescriptorSet;

    std::unique_ptr<PipelineState> _lightingPassPipelineState;

    std::vector<std::unique_ptr<Light>> _lights;

    void ensureRenderTargetSize(size_t width, size_t height);
 public:
    explicit LightingManager(Renderer* renderer);
    ~LightingManager() { };

    Light* addLight(LightType type, bool shadowing);

    PipelineProperties getGeometryProperties() const;

    void removeLight(Light* light);

    void clearLights();

    void updateLightData();

    void beginLightPass(CommandBuffer* cmd);

    void endLightPass(CommandBuffer* cmd);
};
}


