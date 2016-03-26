#pragma once

#include <renderer/Renderer.hpp>
#include <util/Timing.hpp>
#include "AssimpModel.hpp"

class Application {
    std::unique_ptr<AssimpModel> _model;

    glm::mat4 _projMx;
    glm::mat4 _viewMx;
public:
    Application();

    ~Application();

    void initialize(Renderer *renderer);

    void render(Renderer *renderer, Timing *timimg);

    void deinitialize(Renderer *renderer);
};


