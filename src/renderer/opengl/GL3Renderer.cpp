//
//

#include "GL3Renderer.hpp"
#include "GL3BufferObject.hpp"
#include "GL3VertexLayout.hpp"
#include "GL3ShaderProgram.hpp"
#include "GL3DrawCall.hpp"

#include <glad/glad.h>
#include <SDL.h>

namespace {

#ifndef NDEBUG

    void debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                        GLsizei length, const GLchar *message, const void *userParam) {
        const char *sourceStr;
        const char *typeStr;
        const char *severityStr;

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

OGL3Renderer::~OGL3Renderer() {

}

void OGL3Renderer::deinitialize() {
    SDL_GL_DeleteContext(_context);
    _context = nullptr;

    SDL_DestroyWindow(_window);
    _window = nullptr;

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

SDL_Window *OGL3Renderer::initialize(std::unique_ptr<FileLoader> &&fileLoader) {
    SDL_InitSubSystem(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    int context_flags = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
#ifndef NDEBUG
    context_flags |= SDL_GL_CONTEXT_DEBUG_FLAG;
#endif
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, context_flags);

    _window = SDL_CreateWindow("OGL3 Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1680, 1050,
                               SDL_WINDOW_OPENGL);

    if (!_window) {
        return nullptr;
    }

    _context = SDL_GL_CreateContext(_window);

    if (!_context) {
        SDL_DestroyWindow(_window);
        return nullptr;
    }
    SDL_GL_MakeCurrent(_window, _context);
    if (SDL_GL_SetSwapInterval(-1) == -1) {
        SDL_GL_SetSwapInterval(1);
    }

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        deinitialize();
        return nullptr;
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

    int width, height;
    SDL_GL_GetDrawableSize(_window, &width, &height);
    glViewport(0, 0, width, height);

    _fileLoader = std::move(fileLoader);
    _drawCallManager.reset(new GL3DrawCallManager());

    return _window;
}


void OGL3Renderer::presentNextFrame() {
    SDL_GL_SwapWindow(_window);
}

void OGL3Renderer::clear(const glm::vec4 &color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

std::unique_ptr<BufferObject> OGL3Renderer::createBuffer(BufferType type) {
    if (type == BufferType::None) {
        return nullptr;
    }

    return std::unique_ptr<BufferObject>(new GL3BufferObject(type));
}

std::unique_ptr<VertexLayout> OGL3Renderer::createVertexLayout() {
    return std::unique_ptr<VertexLayout>(new GL3VertexLayout());
}

std::unique_ptr<ShaderProgram> OGL3Renderer::createShader(ShaderType type) {
    return std::unique_ptr<ShaderProgram>(new GL3ShaderProgram(_fileLoader.get(), type));
}

SDL_Window *OGL3Renderer::getWindow() {
    return _window;
}

DrawCallManager *OGL3Renderer::getDrawCallManager() {
    return _drawCallManager.get();
}

std::unique_ptr<Texture2D> OGL3Renderer::createTexture() {
    return std::unique_ptr<Texture2D>(new GL3Texture2D());
}



