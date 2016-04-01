#ifndef PROJECT_RENDERER_H
#define PROJECT_RENDERER_H
#pragma once

#include <SDL_video.h>

#include <glm/glm.hpp>

#include <memory>

#include "BufferObject.hpp"
#include "VertexLayout.hpp"
#include "util/FileLoader.hpp"
#include "DrawCall.hpp"
#include "DrawCallManager.hpp"
#include "ShaderParameters.hpp"
#include "LightingManager.hpp"

class Renderer {
public:
    virtual ~Renderer() { }

    virtual SDL_Window *initialize(std::unique_ptr<FileLoader> &&fileLoader) = 0;

    virtual SDL_Window *getWindow() = 0;

    virtual DrawCallManager* getDrawCallManager() = 0;

    virtual LightingManager* getLightingManager() = 0;

    virtual std::unique_ptr<BufferObject> createBuffer(BufferType type) = 0;

    virtual std::unique_ptr<VertexLayout> createVertexLayout() = 0;

    virtual std::unique_ptr<Texture2D> createTexture() = 0;

    virtual std::unique_ptr<PipelineState> createPipelineState(const PipelineProperties& props) = 0;

    virtual void clear(const glm::vec4 &color) = 0;

    virtual void presentNextFrame() = 0;

    virtual void deinitialize() = 0;
};

#endif //PROJECT_RENDERER_H
