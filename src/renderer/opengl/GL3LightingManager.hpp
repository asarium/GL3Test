#pragma once

#include "renderer/LightingManager.hpp"
#include "GL3BufferObject.hpp"
#include "GL3VertexLayout.hpp"
#include "GL3Object.hpp"
#include "GL3ShaderParameters.hpp"

#include <renderer/Renderer.hpp>

#include <memory>
#include <vector>
#include <glad/glad.h>

class GL3LightingManager;

class GL3Light final: public GL3Object, public Light {
    GL3LightingManager* _lightingManager;

    uint32_t _depthMapResolution;

    GL3OwnedTextureHandle _depthTexture;
    GLuint _depthFrameBuffer;

    std::unique_ptr<PipelineState> _shadowPipelineState;

    ShadowMatrices _matrices;

    void freeResources();
    void createDepthBuffer(uint32_t resolution);
 public:
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

    GL3Light(GL3Renderer* renderer, GL3LightingManager* manager, LightType type, uint32_t shadowResolution);
    virtual ~GL3Light();

    LightType type;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;

    void setPosition(const glm::vec3& pos) override;

    void setDirection(const glm::vec3& pos) override;

    void setColor(const glm::vec3& color) override;

    ShadowMatrices beginShadowPass() override;

    void endShadowPass() override;

    bool hasShadow() const {
        return _depthMapResolution != 0;
    }

    void updateShadowMapDescriptor(GL3Descriptor* desc);

    void changeShadowMapResolution(uint32_t resolution);

    void setParameters(LightParameters* params);
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

    std::unique_ptr<BufferObject> _lightUniformBuffer;

    std::unique_ptr<GL3DescriptorSet> _lightingParameterSet;
    GL3Descriptor* _shadowMapDescriptor;

    uint32_t _shadowMapResolution;

    bool _dirty;

    void createFrameBuffer(int width, int height);
    void freeResources();
 public:
    explicit GL3LightingManager(GL3Renderer* renderer);

    virtual ~GL3LightingManager();

    bool initialize();

    void markDirty();

    void resizeFramebuffer(uint32_t width, uint32_t height);

    void changeShadowQuality(SettingsLevel level);

    Light* addLight(LightType type, bool shadows) override;

    void removeLight(Light* light) override;

    void clearLights() override;

    void beginLightPass() override;

    void endLightPass() override;
};


