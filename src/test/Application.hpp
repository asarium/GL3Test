#pragma once

#include <renderer/Renderer.hpp>
#include <util/Timing.hpp>
#include "AssimpModel.hpp"

class Application {
    std::unique_ptr<AssimpModel> _model;

    std::unique_ptr<BufferObject> _vertex_buffer;
    std::unique_ptr<BufferObject> _index_buffer;

    std::unique_ptr<VertexLayout> _vertex_layout;

    std::unique_ptr<ShaderProgram> _shader;

    std::unique_ptr<DrawCall> _drawCall;
public:
    Application();

    ~Application();

    void initialize(Renderer *renderer);

    void render(Renderer *renderer, Timing *timimg);

    void deinitialize(Renderer *renderer);
};


