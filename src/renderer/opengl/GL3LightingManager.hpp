#pragma once

#include "renderer/LightingManager.hpp"
#include "GL3ShaderProgram.hpp"
#include "GL3BufferObject.hpp"
#include "GL3VertexLayout.hpp"
#include "GL3Object.hpp"

#include <memory>
#include <vector>
#include <glad/glad.h>

class GL3Renderer;

class GL3Light : public Light {
public:
    GL3Light(LightType type);

    virtual ~GL3Light() { }

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

class GL3LightingManager : GL3Object, public LightingManager {
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

    std::unique_ptr<BufferObject> _quadVertexBuffer;
    std::unique_ptr<GL3VertexLayout> _quadVertexLayout;

    std::unique_ptr<PipelineState> _geometryPipelineState;

    GL3ShaderProgram *_lightingPassProgram;
    GL3ShaderParameters _lightingPassParameters;

    void createFrameBuffer(int width, int height);
    void freeResources();
public:
    GL3LightingManager(GL3Renderer *renderer);

    virtual ~GL3LightingManager();

    bool initialize();

    void resizeFramebuffer(uint32_t width, uint32_t height);

    virtual Light *addLight(LightType type) override;

    virtual void removeLight(Light *light) override;

    virtual PipelineState *getRenderPipeline() override;

    virtual void clearLights() override;

    virtual void beginLightPass() override;

    virtual void endLightPass() override;
};


