#pragma once

#include "renderer/Renderer.hpp"

#include <SDL_video.h>

class OGL3Renderer : public Renderer {
    SDL_GLContext _context;
    SDL_Window *_window;

    std::unique_ptr<FileLoader> _fileLoader;
public:
    virtual ~OGL3Renderer();

    virtual SDL_Window *initialize(std::unique_ptr<FileLoader>&& fileLoader) override;

    virtual SDL_Window *getWindow() override;

    virtual void clear(const glm::vec4& color) override;

    virtual std::unique_ptr<BufferObject> createBuffer(BufferType type) override;

    virtual std::unique_ptr<VertexLayout> createVertexLayout() override;

    virtual std::unique_ptr<ShaderProgram> createShader(ShaderType type) override;

    virtual std::unique_ptr<DrawCall> createDrawCall(const DrawCallProperties& props) override;

    virtual void presentNextFrame() override;

    virtual void deinitialize() override;
};


