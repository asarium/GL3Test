#pragma once

#include <renderer/Renderer.hpp>
#include <util/Timing.hpp>
#include <SDL_events.h>
#include "AssimpModel.hpp"

#include <glm/glm.hpp>
#include <deque>

struct Particle {
    glm::vec3 position;
    float radius;
};

class Application {
    std::unique_ptr<AssimpModel> _model;

    std::unique_ptr<Texture2D> _particleTexture;
    std::unique_ptr<BufferObject> _particleBuffer;

    std::vector<Light*> _particleLights;
    std::vector<Particle> _particles;

    std::unique_ptr<PipelineState> _particleQuadPipelineState;
    std::unique_ptr<BufferObject> _particleQuadBuffer;
    std::unique_ptr<VertexLayout> _particleQuadLayout;
    std::unique_ptr<InstancedDrawCall> _particleQuadDrawCall;

    std::unique_ptr<Texture2D> _floorTexture;
    std::unique_ptr<DrawCall> _floorDrawCall;

    std::unique_ptr<BufferObject> _fullscreenTriBuffer;
    std::unique_ptr<VertexLayout> _fullscreenTriLayout;
    std::unique_ptr<DrawCall> _fullscreenTriDrawCall;
    std::unique_ptr<PipelineState> _hdrPipelineState;

    std::unique_ptr<RenderTarget> _hdrRenderTarget;

    ProfilingCategory* _wholeFrameCategory;

    Timing *_timing;
    Renderer *_renderer;

    glm::mat4 _projMx;
    glm::mat4 _viewMx;
    glm::mat4 _modelMx;

    Light* _sunLight;

    int _resolution_index = 0;
    bool _last_vsync = true;

    std::unique_ptr<RenderTarget> createHDRRenderTarget(uint32_t width, uint32_t height);

    void changeResolution(uint32_t width, uint32_t height);

    void updateParticles();

    std::deque<float> _cpuTimes;
    std::deque<float> _gpuTimes;
    void renderUI();

    void renderScene(const glm::mat4& projMx, const glm::mat4& viewMx);
public:
    Application(Renderer *renderer, Timing *timimg);

    ~Application();

    void render(Renderer *renderer);

    void handleEvent(SDL_Event *event);
};


