#pragma once

#include <renderer/Renderer.hpp>
#include <util/Timing.hpp>
#include <model/Model.hpp>
#include <SDL_events.h>

#include <renderer/nanovg/nanovg.h>
#include <glm/glm.hpp>
#include <deque>
#include <renderer/nanovg/nanovg_gl.h>
#include "LightingManager.hpp"
#include "DrawCall.hpp"

struct Particle {
    glm::vec3 position;
    float radius;
};

class Application {
    Timing *_timing;
    Renderer *_renderer;
    SDL_Window* _window;

    NVGcontext* _nvgCtx;

    std::unique_ptr<PipelineState> _modelPipelineState;
    std::unique_ptr<Model> _model;

    std::unique_ptr<BufferObject> _floorVertexDataObject;
    std::unique_ptr<VertexArrayObject> _floorVertexArrayObject;
    std::unique_ptr<PipelineState> _floorPipelineState;

    std::unique_ptr<Texture> _floorTexture;
    std::unique_ptr<BufferObject> _floorUniformObject;
    std::unique_ptr<DescriptorSet> _floorModelDescriptorSet;
    DrawCall _floorDrawCall;

    std::unique_ptr<BufferObject> _viewUniformBuffer;
    std::unique_ptr<DescriptorSet> _viewDescriptorSet;

    lighting::LightingManager _lightingManager;

    ProfilingCategory* _wholeFrameCategory;

    ViewUniformData _viewUniforms;

    lighting::Light* _sunLight;

    int _resolution_index = 0;
    bool _last_vsync = true;

    void changeResolution(uint32_t width, uint32_t height);

    std::deque<float> _cpuTimes;
    std::deque<float> _gpuTimes;
    void renderUI();

    void renderScene(CommandBuffer* cmd);
public:
    Application(Renderer *renderer, Timing *timimg, SDL_Window* window);

    ~Application();

    void render(Renderer *renderer);

    void handleEvent(SDL_Event *event);
};


