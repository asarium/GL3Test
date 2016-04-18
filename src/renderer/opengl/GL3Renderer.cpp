//
//

#include "GL3Renderer.hpp"
#include "GL3State.hpp"
#include "EnumTranslation.hpp"

#include <glad/glad.h>

#include <SDL.h>
#include <SDL_video.h>

#include <sstream>

#include <renderer/Exceptions.hpp>
#include <util/Assertion.hpp>

#define NANOVG_GL3_IMPLEMENTATION   // Use GL2 implementation.
#include <renderer/nanovg/nanovg_gl.h>

namespace {

#ifndef NDEBUG

void debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                    GLsizei length, const GLchar* message, const void* userParam) {
    const char* sourceStr;
    const char* typeStr;
    const char* severityStr;

    switch (source) {
        case GL_DEBUG_SOURCE_API_ARB:
            sourceStr = "opengl";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
            sourceStr = "WindowSys";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
            sourceStr = "Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
            sourceStr = "Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION_ARB:
            sourceStr = "Application";
            break;
        case GL_DEBUG_SOURCE_OTHER_ARB:
            sourceStr = "Other";
            break;
        default:
            sourceStr = "Unknown";
            break;
    }

    switch (type) {
        case GL_DEBUG_TYPE_ERROR_ARB:
            typeStr = "Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
            typeStr = "Deprecated behavior";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
            typeStr = "Undefined behavior";
            break;
        case GL_DEBUG_TYPE_PORTABILITY_ARB:
            typeStr = "Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE_ARB:
            typeStr = "Performance";
            break;
        case GL_DEBUG_TYPE_OTHER_ARB:
            typeStr = "Other";
            break;
        default:
            typeStr = "Unknown";
            break;
    }

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH_ARB:
            severityStr = "High";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM_ARB:
            severityStr = "Medium";
            break;
        case GL_DEBUG_SEVERITY_LOW_ARB:
            severityStr = "Low";
            break;
        default:
            severityStr = "Unknown";
            break;
    }

    printf("opengl Debug: Source:%s\tType:%s\tID:%d\tSeverity:%s\tMessage:%s\n",
           sourceStr, typeStr, id, severityStr, message);
}

bool hasPendingDebugMessage() {
    GLint numMsgs = 0;
    glGetIntegerv(GL_DEBUG_LOGGED_MESSAGES_ARB, &numMsgs);

    return numMsgs > 0;
}

bool printNextDebugMessage() {
    if (!hasPendingDebugMessage()) {
        return false;
    }

    GLint msgLen = 0;
    glGetIntegerv(GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB, &msgLen);

    std::vector<GLchar> msg;
    msg.resize(msgLen + 1); // Includes null character, needs to be removed later

    GLenum source;
    GLenum type;
    GLenum severity;
    GLuint id;
    GLsizei length;

    GLuint numFound = glGetDebugMessageLogARB(1, msg.size(), &source, &type, &id, &severity, &length, &msg[0]);

    if (numFound < 1) {
        return false;
    }

    debug_callback(source, type, id, severity, length, msg.data(), nullptr);

    return true;
}

#endif
}

GL3Renderer::GL3Renderer(std::unique_ptr<FileLoader>&& fileLoader) : _fileLoader(std::move(fileLoader)),
                                                                     _settingsManager(this), _window(nullptr),
                                                                     _initialized(false) {

}

GL3Renderer::~GL3Renderer() {

}

void GL3Renderer::deinitialize() {
    nvgDeleteGL3(_nvgContext);
    _nvgContext = nullptr;

    _drawCallManager.reset();
    _lightingManager.reset();
    _shaderManager.reset();
    _renderTargetManager.reset();
    _profiler.reset();
    _util.reset();

    SDL_GL_DeleteContext(_context);
    _context = nullptr;

    SDL_DestroyWindow(_window);
    _window = nullptr;

    SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_TIMER);
}

SDL_Window* GL3Renderer::initialize() {
    SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_TIMER);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    int context_flags = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
#ifndef NDEBUG
    context_flags |= SDL_GL_CONTEXT_DEBUG_FLAG;
#endif
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, context_flags);

    RendererSettings settings;
    auto haveSettings = _settingsManager.getSettings(settings);
    Assertion(haveSettings, "Settings have not been initialized!");

    _window = SDL_CreateWindow("OGL3 Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, settings.resolution.x,
                               settings.resolution.y, SDL_WINDOW_OPENGL);

    if (!_window) {
        std::stringstream ss;
        ss << "Error while creating window: " << SDL_GetError();
        throw RendererException(ss.str());
    }

    _context = SDL_GL_CreateContext(_window);

    if (!_context) {
        SDL_DestroyWindow(_window);

        std::stringstream ss;
        ss << "Error while creating OpenGL context: " << SDL_GetError();
        throw RendererException(ss.str());
    }
    SDL_GL_MakeCurrent(_window, _context);
    SDL_GL_SetSwapInterval(settings.vertical_sync ? 1 : 0);

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        deinitialize();
        throw RendererException("Failed to load OpenGL function pointers!");
    }

#ifndef NDEBUG
    // Set up the debug extension if present
    if (GLAD_GL_ARB_debug_output) {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        GLuint unusedIds = 0;
        glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, GL_TRUE);

        glDebugMessageCallbackARB((GLDEBUGPROCARB) debug_callback, nullptr);

        // Now print all pending log messages
        while (hasPendingDebugMessage()) {
            printNextDebugMessage();
        }
    }
#endif
    GLState.reset(new GL3StateTracker());
    _util.reset(new GL3Util(this));
    _profiler.reset(new GL3Profiler(this));

    _shaderManager.reset(new GL3ShaderManager(_fileLoader.get()));
    // Preload the shaders
    for (auto type : getDefinedShaderTypes()) {
        _shaderManager->getShader(type);
    }

    _drawCallManager.reset(new GL3DrawCallManager(_shaderManager.get()));
    _lightingManager.reset(new GL3LightingManager(this));
    _lightingManager->initialize();

    _lightingManager->changeShadowQuality(_settingsManager.getCurrentSettings().shadow_quality);

    _renderTargetManager.reset(new GL3RenderTargetManager(this));

    updateResolution(settings.resolution.x, settings.resolution.y);

    _nvgContext = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);

    _initialized = true;

    return _window;
}

void GL3Renderer::updateResolution(uint32_t width, uint32_t height) {
    _lightingManager->resizeFramebuffer(width, height);

    _renderTargetManager->updateDefaultTarget(width, height);
    _renderTargetManager->useRenderTarget(nullptr); // Setup correct view port
}

RendererSettingsManager* GL3Renderer::getSettingsManager() {
    return &_settingsManager;
}

void GL3Renderer::presentNextFrame() {
    SDL_GL_SwapWindow(_window);
}

void GL3Renderer::clear(const glm::vec4& color) {
    glClearColor(color.r, color.g, color.b, color.a);
    GLState->setDepthMask(true);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

std::unique_ptr<BufferObject> GL3Renderer::createBuffer(BufferType type) {
    if (type == BufferType::None) {
        return nullptr;
    }

    return std::unique_ptr<BufferObject>(new GL3BufferObject(type));
}

std::unique_ptr<VertexLayout> GL3Renderer::createVertexLayout() {
    return std::unique_ptr<VertexLayout>(new GL3VertexLayout());
}

DrawCallManager* GL3Renderer::getDrawCallManager() {
    return _drawCallManager.get();
}

LightingManager* GL3Renderer::getLightingManager() {
    return _lightingManager.get();
}

std::unique_ptr<Texture2D> GL3Renderer::createTexture() {
    return GL3Texture2D::createTexture();
}

std::unique_ptr<PipelineState> GL3Renderer::createPipelineState(const PipelineProperties& props) {
    GL3PipelineProperties gl_props;
    gl_props.shader = _shaderManager->getShader(convertShaderType(props.shaderType));

    gl_props.blending = props.blending;
    gl_props.blendFunction = props.blendFunction;
    gl_props.depthMode = props.depthMode;
    gl_props.depthFunction = props.depthFunction;

    return std::unique_ptr<PipelineState>(new GL3PipelineState(gl_props));
}

GL3ShaderManager* GL3Renderer::getShaderManager() {
    return _shaderManager.get();
}

RenderTargetManager* GL3Renderer::getRenderTargetManager() {
    return _renderTargetManager.get();
}

GL3DrawCallManager* GL3Renderer::getGLDrawCallManager() {
    return _drawCallManager.get();
}

GL3LightingManager* GL3Renderer::getGLLightingManager() {
    return _lightingManager.get();
}

GL3RenderTargetManager* GL3Renderer::getGLRenderTargetManager() {
    return _renderTargetManager.get();
}

SDL_Window* GL3Renderer::getWindow() {
    return _window;
}

bool GL3Renderer::hasCapability(GraphicsCapability capability) const {
    switch (capability) {
        case GraphicsCapability::PointSprites:
            return true; // Supported through geometry shaders
        case GraphicsCapability::S3TC:
            return GLAD_GL_EXT_texture_compression_s3tc != 0; // Supported if the extension is present
        case GraphicsCapability::Instancing:
            return true; // OpenGL 3 always supports instancing
        case GraphicsCapability::FloatingPointTextures:
            return true; // OpenGL 3 core has support for this
        default:
            return false; // Everything else is not supported
    }
}

Profiler* GL3Renderer::getProfiler() {
    return _profiler.get();
}

NVGcontext* GL3Renderer::getNanovgContext() {
    return _nvgContext;
}

void GL3Renderer::nanovgEndFrame() {
    nvgEndFrame(_nvgContext);

    // Communicate the changes to out state tracker
    GLState->setBlendMode(true);
    GLState->setBlendFunc(BlendFunction::AdditiveAlpha);
    GLState->setDepthTest(false);

    GLState->bindVertexArray(0);

    GLState->Buffer.bindArrayBuffer(0);
    GLState->Program.use(nullptr);

    GLState->Texture.setActiveUnit(0);
}

GL3Renderer::GL3RenderSettingsManager::GL3RenderSettingsManager(GL3Renderer* renderer) : GL3Object(renderer),
                                                                                         _settingsSet(false) {
}

void GL3Renderer::GL3RenderSettingsManager::changeResolution(uint32_t width, uint32_t height) {
    if (_renderer->getWindow() == nullptr) {
        return; // No window has been created yet
    }

    SDL_SetWindowSize(_renderer->getWindow(), width, height);
    if (SDL_GetWindowFlags(_renderer->getWindow()) & SDL_WINDOW_FULLSCREEN) {
        // In fullscreen mode the normal method doesn't work
        SDL_DisplayMode target;
        target.w = width;
        target.h = height;
        target.format = 0; // don't care
        target.refresh_rate = 0; // dont't care
        target.driverdata = 0; // initialize to 0

        SDL_DisplayMode closest;

        if (SDL_GetClosestDisplayMode(0, &target, &closest) != nullptr) {
            // I haven't found a perfect solution for changing resolution when in fullscreen mode
            // On Windows simply calling SDL_SetWindowDisplayMode is enough but on Linux (with GNOME) it just doesn't work
            // This workaround works for linux but is suboptimal for Windows

            SDL_SetWindowFullscreen(_renderer->getWindow(), 0);

            SDL_SetWindowDisplayMode(_renderer->getWindow(), &closest);

            SDL_Delay(250);

            SDL_SetWindowFullscreen(_renderer->getWindow(), SDL_WINDOW_FULLSCREEN);
        }
    }

    _renderer->updateResolution(width, height);
}

bool GL3Renderer::GL3RenderSettingsManager::supportsSetting(SettingsParameter parameter) const {
    switch (parameter) {
        case SettingsParameter::Resolution:
            return true; // Changing resolution is supported
        case SettingsParameter::VerticalSync:
            return true; // Changing vsync is supported
        case SettingsParameter::Shadows:
            return true; // Shadows are implemented using shadow mapping
        default:
            return false; // Unknown option is not supported
    }
}

void GL3Renderer::GL3RenderSettingsManager::changeSettings(const RendererSettings& settings) {
    auto previousSettings = _currentSettings;
    _currentSettings = settings;

    _settingsSet = true;

    if (_renderer->_initialized) {
        // Only change settings if the renderer has actually been initialized
        if (_currentSettings.resolution != previousSettings.resolution) {
            changeResolution(_currentSettings.resolution.x, _currentSettings.resolution.y);
        }

        if (_currentSettings.vertical_sync != previousSettings.vertical_sync) {
            SDL_GL_SetSwapInterval(_currentSettings.vertical_sync ? 1 : 0);
        }

        if (_currentSettings.shadow_quality != previousSettings.shadow_quality) {
            _renderer->_lightingManager->changeShadowQuality(_currentSettings.shadow_quality);
        }
    }
}

bool GL3Renderer::GL3RenderSettingsManager::getSettings(RendererSettings& settings) {
    settings = _currentSettings;

    return _settingsSet;
}

RendererSettings GL3Renderer::GL3RenderSettingsManager::getCurrentSettings() const {
    return _currentSettings;
}
