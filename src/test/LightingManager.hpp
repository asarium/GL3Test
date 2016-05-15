#pragma once

#include <renderer/Renderer.hpp>
#include <util/UniformAligner.hpp>
#include "DrawUtil.hpp"

namespace lighting {

struct LightParameters
{
    glm::mat4 light_view_proj_matrix;
    glm::mat4 model_matrix;

    glm::vec3 light_vector;
    int32_t light_type;
    glm::vec3 light_color;
    uint32_t light_has_shadow;

    glm::vec2 frag_coord_scale;
};

enum class LightType {
    Directional,
    Point,
    Ambient
};

class LightingManager;

class Light {
    Renderer* _renderer;
    LightingManager* _manager;

    LightType _type;
    glm::vec3 _position;
    glm::vec3 _direction;
    glm::vec3 _color;
 public:
    Light(Renderer* renderer, LightingManager* manager, LightType type);
    ~Light() { }

    void setPosition(const glm::vec3& pos);

    void setDirection(const glm::vec3& dir);

    void setColor(const glm::vec3& color);

    void setParameters(LightParameters* params);

    LightType getType();
};

class LightingManager {
    Renderer* _renderer;

    DrawUtil _util;

    std::unique_ptr<RenderTarget> _lightingRenderTarget;

    std::unique_ptr<BufferObject> _uniformBuffer;

    UniformAligner<LightParameters> _alignedUniformData;

    std::unique_ptr<DescriptorSet> _lightingDescriptorSet;
    Descriptor* _lightingDataDescriptor;

    std::unique_ptr<PipelineState> _geometryPipelinestate;
    std::unique_ptr<PipelineState> _lightingPassPipelineState;

    std::vector<std::unique_ptr<Light>> _lights;

    void ensureRenderTargetSize(size_t width, size_t height);
 public:
    LightingManager(Renderer* renderer);
    ~LightingManager() { };

    Light* addLight(LightType type, bool shadowing);

    void removeLight(Light* light);

    void clearLights();

    void updateLightData();

    void beginLightPass();

    void endLightPass();
};
}


