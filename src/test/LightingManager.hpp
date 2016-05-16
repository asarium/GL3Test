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

struct ShadowMatrices {
    glm::mat4 projection;
    glm::mat4 view;
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

    bool _shadowing;
    std::unique_ptr<PipelineState> _shadowPassPipelinestate;
    std::unique_ptr<DescriptorSet> _lightDescriptorSet;
    Descriptor* _uniformDescriptor;

    std::unique_ptr<RenderTarget> _shadowMapTarget;

    ShadowMatrices _matricies;
 public:
    Light(Renderer* renderer, LightingManager* manager, LightType type, bool shadowing);
    ~Light() { }

    void setPosition(const glm::vec3& pos);

    void setDirection(const glm::vec3& dir);

    void setColor(const glm::vec3& color);

    void setParameters(LightParameters* params);

    LightType getType();

    void updateDescriptor(BufferObject* uniforms, size_t offset, size_t size);

    void bindDescriptorSet();
    void unbindDescriptorSet();

    ShadowMatrices beginShadowPass();

    void endShadowPass();
};

class LightingManager {
    Renderer* _renderer;

    DrawUtil _util;

    std::unique_ptr<RenderTarget> _lightingRenderTarget;

    std::unique_ptr<BufferObject> _uniformBuffer;

    UniformAligner<LightParameters> _alignedUniformData;

    std::unique_ptr<DescriptorSet> _lightingDescriptorSet;

    std::unique_ptr<PipelineState> _geometryPipelinestate;
    std::unique_ptr<PipelineState> _lightingPassPipelineState;

    std::vector<std::unique_ptr<Light>> _lights;

    void ensureRenderTargetSize(size_t width, size_t height);
 public:
    explicit LightingManager(Renderer* renderer);
    ~LightingManager() { };

    Light* addLight(LightType type, bool shadowing);

    void removeLight(Light* light);

    void clearLights();

    void updateLightData();

    void beginLightPass();

    void endLightPass();
};
}


