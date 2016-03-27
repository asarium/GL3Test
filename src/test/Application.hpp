#pragma once

#include <renderer/Renderer.hpp>
#include <util/Timing.hpp>
#include <SDL_events.h>
#include "AssimpModel.hpp"

class Application {
    std::unique_ptr<AssimpModel> _model;
    Timing* _timing;

    glm::mat4 _projMx;
    glm::mat4 _viewMx;
    glm::mat4 _modelMx;

    float xAngle;
    float yAngle;

    void updateModelMatrix();
public:
    Application();

    ~Application();

    void initialize(Renderer *renderer, Timing *timimg);

    void render(Renderer *renderer);

    void handleEvent(SDL_Event* event);

    void deinitialize(Renderer *renderer);
};


