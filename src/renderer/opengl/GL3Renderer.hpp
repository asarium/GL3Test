#pragma once

#include "renderer/Renderer.hpp"
#include "GL3DrawCallManager.hpp"
#include "GL3LightingManager.hpp"
#include "GL3ShaderManager.hpp"
#include "GL3RenderTargetManager.hpp"

#include <SDL_video.h>

class GL3Renderer : public Renderer {
public:
    class GL3RenderSettings : GL3Object, public RendererSettings {
    public:
        GL3RenderSettings(GL3Renderer* renderer);

        virtual ~GL3RenderSettings() { }

        virtual void changeResolution(uint32_t width, uint32_t height) override;

        virtual bool supportsOption(SettingsParameter param, SettingsParameterType *parameterType) override;

        virtual void setOption(SettingsParameter param, void *value) override;
    };

private:

    GL3RenderSettings _settings;

    SDL_GLContext _context;
    SDL_Window *_window;

    std::unique_ptr<FileLoader> _fileLoader;

    std::unique_ptr<GL3DrawCallManager> _drawCallManager;
    std::unique_ptr<GL3LightingManager> _lightingManager;
    std::unique_ptr<GL3ShaderManager> _shaderManager;
    std::unique_ptr<GL3RenderTargetManager> _renderTargetManager;
public:
    GL3Renderer(std::unique_ptr<FileLoader> &&fileLoader);

    virtual ~GL3Renderer();

    virtual SDL_Window *initialize(uint32_t width, uint32_t height) override;

    virtual void deinitialize() override;

    virtual RendererSettings* getSettings() override;

    virtual DrawCallManager *getDrawCallManager() override;

    virtual LightingManager *getLightingManager() override;

    virtual RenderTargetManager *getRenderTargetManager() override;

    virtual std::unique_ptr<BufferObject> createBuffer(BufferType type) override;

    virtual std::unique_ptr<VertexLayout> createVertexLayout() override;

    virtual std::unique_ptr<Texture2D> createTexture() override;

    virtual std::unique_ptr<PipelineState> createPipelineState(const PipelineProperties &props) override;

    virtual bool hasCapability(GraphicsCapability capability) override;

    virtual void clear(const glm::vec4 &color) override;

    virtual void presentNextFrame() override;

    void updateResolution(uint32_t width, uint32_t height);

    SDL_Window* getWindow();

    GL3DrawCallManager *getGLDrawCallManager();

    GL3LightingManager *getGLLightingManager();

    GL3RenderTargetManager *getGLRenderTargetManager();

    GL3ShaderManager *getShaderManager();
};


