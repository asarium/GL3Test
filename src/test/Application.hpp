#pragma once

#include <renderer/Renderer.hpp>
#include <util/Timing.hpp>
#include <model/Model.hpp>
#include <SDL_events.h>

#include <glm/glm.hpp>
#include <deque>

struct Particle {
    glm::vec3 position;
    float radius;
};

class Application {
    std::unique_ptr<Model> _model;

    std::unique_ptr<BufferObject> _floorVertexDataObject;
    std::unique_ptr<VertexLayout> _floorVertexLayout;
    std::unique_ptr<Texture2D> _floorTexture;
    std::unique_ptr<DrawCall> _floorDrawCall;

    std::unique_ptr<DescriptorSet> _floorModelDescriptorSet;

    std::unique_ptr<BufferObject> _fullscreenTriBuffer;
    std::unique_ptr<VertexLayout> _fullscreenTriLayout;
    std::unique_ptr<DrawCall> _fullscreenTriDrawCall;
    std::unique_ptr<PipelineState> _hdrPipelineState;

    std::unique_ptr<RenderTarget> _bloomRenderTargets[2];

    std::unique_ptr<PipelineState> _brightPassState;
    std::unique_ptr<PipelineState> _bloomPassState;

    std::unique_ptr<RenderTarget> _hdrRenderTarget;

    std::unique_ptr<BufferObject> _viewUniformBuffer;
    std::unique_ptr<DescriptorSet> _viewDescriptorSet;

    ProfilingCategory* _wholeFrameCategory;

    Timing *_timing;
    Renderer *_renderer;

    ViewUniformData _viewUniforms;

    Light* _sunLight;

    int _resolution_index = 0;
    bool _last_vsync = true;

    std::unique_ptr<RenderTarget> createHDRRenderTarget(uint32_t width, uint32_t height);

    void changeResolution(uint32_t width, uint32_t height);

    void updateParticles();

    std::deque<float> _cpuTimes;
    std::deque<float> _gpuTimes;
    void renderUI();

    void renderScene();

    Texture2DHandle* doBloomPass();
public:
    Application(Renderer *renderer, Timing *timimg);

    ~Application();

    void render(Renderer *renderer);

    void handleEvent(SDL_Event *event);
};


