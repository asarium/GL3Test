#pragma once

#include "renderer/Renderer.hpp"
#include "GL3DrawCallManager.hpp"
#include "GL3LightingManager.hpp"
#include "GL3ShaderManager.hpp"
#include "GL3RenderTargetManager.hpp"
#include "GL3Util.hpp"
#include "GL3Profiler.hpp"
#include "GL3PushConstantManager.hpp"

#include <SDL_video.h>

class GL3Renderer final: public Renderer {
 public:
    class GL3RenderSettingsManager: GL3Object, public RendererSettingsManager {
        void changeResolution(uint32_t width, uint32_t height);

        bool _settingsSet;
        RendererSettings _currentSettings;
     public:
        GL3RenderSettingsManager(GL3Renderer* renderer);

        virtual ~GL3RenderSettingsManager() { }

        bool getSettings(RendererSettings& settings);

        virtual void changeSettings(const RendererSettings& settings) override;

        virtual bool supportsSetting(SettingsParameter parameter) const override;

        virtual RendererSettings getCurrentSettings() const override;
    };

 private:

    std::unique_ptr<FileLoader> _fileLoader;

    GL3RenderSettingsManager _settingsManager;

    SDL_Window* _window;
    bool _initialized;
    SDL_GLContext _context;

    std::unique_ptr<GL3DrawCallManager> _drawCallManager;
    std::unique_ptr<GL3LightingManager> _lightingManager;
    std::unique_ptr<GL3ShaderManager> _shaderManager;
    std::unique_ptr<GL3RenderTargetManager> _renderTargetManager;
    std::unique_ptr<GL3Profiler> _profiler;
    std::unique_ptr<GL3Util> _util;
    std::unique_ptr<GL3PushConstantManager> _pushConstantManager;

    NVGcontext* _nvgContext;
 public:
    GL3Renderer(std::unique_ptr<FileLoader>&& fileLoader);

    virtual ~GL3Renderer();

    virtual SDL_Window* initialize() override;

    virtual void deinitialize() override;

    virtual RendererSettingsManager* getSettingsManager() override;

    virtual DrawCallManager* getDrawCallManager() override;

    virtual LightingManager* getLightingManager() override;

    virtual RenderTargetManager* getRenderTargetManager() override;

    virtual Profiler* getProfiler() override;

    virtual std::unique_ptr<BufferObject> createBuffer(BufferType type) override;

    virtual std::unique_ptr<VertexLayout> createVertexLayout() override;

    virtual std::unique_ptr<Texture2D> createTexture() override;

    virtual std::unique_ptr<PipelineState> createPipelineState(const PipelineProperties& props) override;

    virtual std::unique_ptr<DescriptorSet> createDescriptorSet(DescriptorSetType type) override;

    virtual bool hasCapability(GraphicsCapability capability) const override;

    virtual void clear(const glm::vec4& color) override;

    virtual void presentNextFrame() override;

    virtual NVGcontext* getNanovgContext() override;

    virtual void nanovgEndFrame() override;

    void updateResolution(uint32_t width, uint32_t height);

    int getNanoVGImageHandle(GLuint tex_handle, GLsizei width, GLsizei height);

    SDL_Window* getWindow();

    GL3DrawCallManager* getGLDrawCallManager();

    GL3LightingManager* getGLLightingManager();

    GL3RenderTargetManager* getGLRenderTargetManager();

    GL3ShaderManager* getShaderManager();

    GL3PushConstantManager* getPushConstantManager();

    GL3Util* getGLUtil() {
        return _util.get();
    }
};


