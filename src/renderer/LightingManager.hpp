#pragma once

#include <glm/glm.hpp>

#include "PipelineState.hpp"

enum class LightType {
    Directional,
    Point,
    Ambient
};

struct ShadowMatrices {
    glm::mat4 projection;
    glm::mat4 view;
};

class Light {
 public:
    virtual ~Light() { }

    virtual void setPosition(const glm::vec3& pos) = 0;

    virtual void setDirection(const glm::vec3& dir) = 0;

    virtual void setColor(const glm::vec3& color) = 0;

    virtual ShadowMatrices beginShadowPass() = 0;

    virtual void endShadowPass() = 0;

    virtual PipelineState* getShadowPipelineState() = 0;
};

class LightingManager {
 public:
    virtual ~LightingManager() { };

    virtual Light* addLight(LightType type, bool lighting) = 0;

    virtual PipelineState* getRenderPipeline() = 0;

    virtual void removeLight(Light* light) = 0;

    virtual void clearLights() = 0;

    virtual void beginLightPass(const glm::mat4& projection, const glm::mat4& view) = 0;

    virtual void endLightPass() = 0;
};


