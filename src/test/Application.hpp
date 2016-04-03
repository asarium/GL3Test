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

    Timing* _timing;

    glm::mat4 _projMx;
    glm::mat4 _viewMx;
    glm::mat4 _modelMx;
public:
    Application();

    ~Application();

    void initialize(Renderer *renderer, Timing *timimg);

    void render(Renderer *renderer);

    void handleEvent(SDL_Event* event);

    void deinitialize(Renderer *renderer);
};


