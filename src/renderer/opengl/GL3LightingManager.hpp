#pragma once

#include "renderer/LightingManager.hpp"

#include <memory>
#include <vector>
#include <glad/glad.h>

class GL3Renderer;

class GL3Light : public Light {
public:
    GL3Light(LightType type);
    virtual ~GL3Light() {}

    virtual void setPosition(const glm::vec3 &pos) override;

    virtual void setDirection(const glm::vec3 &pos) override;

    virtual void setColor(const glm::vec3 &color) override;

    LightType type;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
};

class GL3LightingManager : public LightingManager {
    std::vector<std::unique_ptr<GL3Light>> _lights;

    GLuint _renderFrameBuffer;

    GL3Renderer* _renderer;
public:
    GL3LightingManager(GL3Renderer* renderer);
    virtual ~GL3LightingManager() {}

    virtual Light *addLight(LightType type) override;

    virtual void removeLight(Light* light) override;

    virtual void clearLights() override;

    virtual void beginLightPass() override;

    virtual void endLightPass() override;
};


