#pragma once

#include "renderer/LightingManager.hpp"
#include "GL3ShaderProgram.hpp"
#include "GL3BufferObject.hpp"
#include "GL3VertexLayout.hpp"

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

    virtual void setIntesity(float intensity) override;

    LightType type;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
};

class GL3LightingManager : public LightingManager {
    enum GBuffer {
        ALBEDO_BUFFER = 0,
        POSITION_BUFFER = 1,
        NORMAL_BUFFER = 2,
        DEPTH_BUFFER = 3,

        NUM_GBUFFERS
    };

    std::vector<std::unique_ptr<GL3Light>> _lights;

    GLuint _renderFrameBuffer;

    GLuint _gBufferTextures[NUM_GBUFFERS];

    GL3Renderer* _renderer;

    std::unique_ptr<BufferObject> _quadVertexBuffer;
    std::unique_ptr<GL3VertexLayout> _quadVertexLayout;

    GL3ShaderProgram* _lightingPassProgram;
    GL3ShaderParameters _lightingPassParameters;
public:
    GL3LightingManager(GL3Renderer* renderer);
    virtual ~GL3LightingManager();

    bool initialize(int width, int height);

    virtual Light *addLight(LightType type) override;

    virtual void removeLight(Light* light) override;

    virtual void clearLights() override;

    virtual void beginLightPass() override;

    virtual void endLightPass() override;
};


