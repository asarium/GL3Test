//
//

#include "GL3Renderer.hpp"
#include "GL3State.hpp"
#include "EnumTranslation.hpp"
#include "GL3ShaderDefintions.hpp"
#include "GL3CommandBuffer.hpp"
#include "renderer/Exceptions.hpp"
#include "util/Assertion.hpp"
#include "GL3PipelineState.hpp"

#include <glad/glad.h>

#include <SDL.h>
#include <SDL_video.h>

#include <sstream>


namespace {

#ifndef NDEBUG

void debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                    GLsizei length, const GLchar* message, const void* userParam) {
    if (source == GL_DEBUG_SOURCE_APPLICATION_ARB) {
        // Ignore application messages
        return;
    }

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

void glad_noop_callback(const char* name, void* funcptr, int len_args, ...) {
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
    _shaderManager.reset();
    _renderTargetManager.reset();
    _profiler.reset();
    _pushConstantManager.reset();
    _debugging.reset();

    SDL_GL_DeleteContext(_context);
    _context = nullptr;

    _window = nullptr;

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void GL3Renderer::initialize(SDL_Window* window) {
    Assertion(window, "Invalid window handle passed!");

    _window = window;

    SDL_InitSubSystem(SDL_INIT_VIDEO);

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

    _context = SDL_GL_CreateContext(_window);

    if (!_context) {
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
        glad_set_post_callback(glad_noop_callback); // Disable callback based error checking

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
    _pushConstantManager.reset(new GL3PushConstantManager(this));
    _profiler.reset(new GL3Profiler(this));
    _debugging.reset(new GL3Debugging());

    _shaderManager.reset(new GL3ShaderManager(_fileLoader.get()));
    // Preload the shaders
    for (auto type : getDefinedShaderTypes()) {
        _shaderManager->getProgram(type.first, type.second);
    }

    _renderTargetManager.reset(new GL3RenderTargetManager(this));

    updateResolution(settings.resolution.x, settings.resolution.y);

    _initialized = true;
}

void GL3Renderer::updateResolution(uint32_t width, uint32_t height) {
    _renderTargetManager->updateDefaultTarget(width, height);
    _renderTargetManager->useRenderTarget(nullptr); // Setup correct view port
}

RendererSettingsManager* GL3Renderer::getSettingsManager() {
    return &_settingsManager;
}

void GL3Renderer::presentNextFrame() {
    SDL_GL_SwapWindow(_window);
}

std::unique_ptr<BufferObject> GL3Renderer::createBuffer(BufferType type) {
    if (type == BufferType::None) {
        return nullptr;
    }

    return std::unique_ptr<BufferObject>(new GL3BufferObject(type));
}

std::unique_ptr<CommandBuffer> GL3Renderer::createCommandBuffer() {
    return std::unique_ptr<CommandBuffer>(new GL3CommandBuffer(this));
}

std::unique_ptr<Texture> GL3Renderer::createTexture() {
    return GL3Texture::createTexture(this);
}

std::unique_ptr<PipelineState> GL3Renderer::createPipelineState(const PipelineProperties& props) {
    return std::unique_ptr<PipelineState>(new GL3PipelineState(this, props));
}

std::unique_ptr<DescriptorSet> GL3Renderer::createDescriptorSet(DescriptorSetType type) {
    return std::unique_ptr<DescriptorSet>(new GL3DescriptorSet(convertDescriptorSetType(type)));
}

GL3ShaderManager* GL3Renderer::getShaderManager() {
    return _shaderManager.get();
}

RenderTargetManager* GL3Renderer::getRenderTargetManager() {
    return _renderTargetManager.get();
}

GL3RenderTargetManager* GL3Renderer::getGLRenderTargetManager() {
    return _renderTargetManager.get();
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

GL3PushConstantManager* GL3Renderer::getPushConstantManager() {
    return _pushConstantManager.get();
}
RendererLimits GL3Renderer::getLimits() const {
    RendererLimits limits;

    limits.uniform_offset_alignment = (size_t) GLState->Constants.getUniformBufferAlignment();

    return limits;
}
std::unique_ptr<VertexArrayObject> GL3Renderer::createVertexArrayObject(const VertexInputStateProperties& input,
                                                                        const VertexArrayProperties& props) {
    GL3VertexInputState inputState(input);
    return inputState.createArrayObject(props);
}
Debugging* GL3Renderer::getDebugging() {
    return _debugging.get();
}

GL3Renderer::GL3RenderSettingsManager::GL3RenderSettingsManager(GL3Renderer* renderer) : GL3Object(renderer),
                                                                                         _settingsSet(false) {
}

void GL3Renderer::GL3RenderSettingsManager::changeResolution(uint32_t width, uint32_t height) {
    _renderer->updateResolution(width, height);
}

bool GL3Renderer::GL3RenderSettingsManager::supportsSetting(SettingsParameter parameter) const {
    switch (parameter) {
        case SettingsParameter::Resolution:
            return true; // Changing resolution is supported
        case SettingsParameter::VerticalSync:
            return true; // Changing vsync is supported
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
    }
}

bool GL3Renderer::GL3RenderSettingsManager::getSettings(RendererSettings& settings) {
    settings = _currentSettings;

    return _settingsSet;
}

RendererSettings GL3Renderer::GL3RenderSettingsManager::getCurrentSettings() const {
    return _currentSettings;
}
