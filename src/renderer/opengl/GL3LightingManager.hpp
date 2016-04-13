#pragma once

#include "renderer/LightingManager.hpp"
#include "GL3ShaderProgram.hpp"
#include "GL3BufferObject.hpp"
#include "GL3VertexLayout.hpp"
#include "GL3Object.hpp"

#include <renderer/Renderer.hpp>

#include <memory>
#include <vector>
#include <glad/glad.h>

class GL3LightingManager;

class GL3Light: public GL3Object, public Light {
    GL3LightingManager* _lightingManager;

    uint32_t _depthMapResolution;

    GLuint _depthTexture;
    GLuint _depthFrameBuffer;

    std::unique_ptr<PipelineState> _shadowPipelineState;

    ShadowMatrices _matrices;

    void freeResources();
    void createDepthBuffer(uint32_t resolution);
 public:
    GL3Light(GL3Renderer* renderer, GL3LightingManager* manager, LightType type, uint32_t shadowResolution);
    virtual ~GL3Light();

    LightType type;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;

    virtual void setPosition(const glm::vec3& pos) override;

    virtual void setDirection(const glm::vec3& pos) override;

    virtual void setColor(const glm::vec3& color) override;

    virtual ShadowMatrices beginShadowPass() override;

    virtual void endShadowPass() override;

    bool hasShadow() {
        return _depthMapResolution != 0;
    }

    void changeShadowMapResolution(uint32_t resolution);

    void setParameters(GL3ShaderParameters* params);
};

class GL3LightingManager: GL3Object, public LightingManager {
    enum GBuffer {
        POSITION_BUFFER = 0,
        NORMAL_BUFFER = 1,
        ALBEDO_BUFFER = 2,

        NUM_GBUFFERS
    };

    glm::uvec2 _framebufferSize;

    std::vector<std::unique_ptr<GL3Light>> _lights;

    GLuint _renderFrameBuffer;

    GLuint _gBufferTextures[NUM_GBUFFERS];

    GLuint _depthRenderBuffer;

    std::unique_ptr<PipelineState> _geometryPipelineState;

    GL3ShaderProgram* _lightingPassProgram;
    GL3ShaderParameters _lightingPassParameters;

    glm::mat4 _projectionMatrix;
    glm::mat4 _viewMatrix;

    uint32_t _shadowMapResolution;

    void createFrameBuffer(int width, int height);
    void freeResources();
 public:
    GL3LightingManager(GL3Renderer* renderer);

    virtual ~GL3LightingManager();

    bool initialize();

    void resizeFramebuffer(uint32_t width, uint32_t height);

    void changeShadowQuality(SettingsLevel level);

    virtual Light* addLight(LightType type, bool shadows) override;

    virtual void removeLight(Light* light) override;

    virtual void clearLights() override;

    virtual void beginLightPass(const glm::mat4& projection, const glm::mat4& view) override;

    virtual void endLightPass() override;
};


