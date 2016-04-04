#pragma once

#include <renderer/Renderer.hpp>
#include <util/Timing.hpp>
#include <SDL_events.h>
#include "AssimpModel.hpp"

class Application {
    std::unique_ptr<AssimpModel> _model;

    std::unique_ptr<RenderTarget> _renderTarget;
    std::unique_ptr<BufferObject> _quadObject;
    std::unique_ptr<VertexLayout> _quadLayout;
    std::unique_ptr<PipelineState> _quadPipelineState;
    std::unique_ptr<DrawCall> _quadDrawCall;

    std::unique_ptr<Texture2D> _copyTexture;

    Timing *_timing;
    Renderer *_renderer;

    glm::mat4 _projMx;
    glm::mat4 _viewMx;
    glm::mat4 _modelMx;

    int _resolution_index = 0;
    bool _last_vsync = true;

    void changeResolution(uint32_t width, uint32_t height);

public:
    Application(Renderer *renderer, Timing *timimg);

    ~Application();

    void render(Renderer *renderer);

    void handleEvent(SDL_Event *event);

    void deinitialize(Renderer *renderer);

};


