#pragma once

#include <renderer/Renderer.hpp>
#include <util/Timing.hpp>
#include <SDL_events.h>
#include "AssimpModel.hpp"

#include <glm/glm.hpp>

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

    ProfilingCategory* _geometryCategory;
    ProfilingCategory* _endLightPassCategory;
    ProfilingCategory* _particleCategory;

    Timing *_timing;
    Renderer *_renderer;

    glm::mat4 _projMx;
    glm::mat4 _viewMx;
    glm::mat4 _modelMx;

    int _resolution_index = 0;
    bool _last_vsync = true;

    void changeResolution(uint32_t width, uint32_t height);

    void updateParticles();
public:
    Application(Renderer *renderer, Timing *timimg);

    ~Application();

    void render(Renderer *renderer);

    void handleEvent(SDL_Event *event);
};


