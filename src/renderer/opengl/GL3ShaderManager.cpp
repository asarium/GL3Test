//
//

#include "GL3ShaderManager.hpp"
#include "GL3ShaderDefintions.hpp"
#include "GL3State.hpp"
#include <renderer/Renderer.hpp>

namespace {
template<typename T, size_t size>
constexpr size_t array_size(const T(&)[size])
{
    return size;
}

std::vector<GLuint> compileShaderParts(FileLoader* loader, const std::vector<ShaderFilename>& parts, const char* header) {
    std::vector<GLuint> compiled_parts;
    compiled_parts.reserve(parts.size());

    for (auto& filename : parts) {
        printf("Compiling %s...\n", filename.filename);
        auto handle = glCreateShader(filename.type);

        auto content = loader->getFileContents(filename.filename);
        if (content.empty())
        {
            throw RendererException("No shader content found!");
        }

        const GLchar* contentStr = reinterpret_cast<GLchar*>(content.data());
        GLint length = static_cast<GLint>(content.size());

        const GLchar* sources[] = {
            reinterpret_cast<const GLchar*>(header),
            contentStr
        };
        GLint sizes[] = {
            static_cast<GLint>(std::strlen(header)),
            length
        };

        static_assert(array_size(sources) == array_size((sizes)), "Sizes of source and size array do not match!");
        auto numParts = static_cast<GLsizei>(array_size(sources));

        glShaderSource(handle, numParts, sources, sizes);

        glCompileShader(handle);

        compiled_parts.push_back(handle);
#ifndef NDEBUG
        GLint logSize = 0;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logSize);

        if (logSize > 1) {
            std::string info_log;
            info_log.resize(logSize);

            glGetShaderInfoLog(handle, logSize, &logSize, &info_log[0]);

            printf("Shader info log: %s\n", info_log.c_str());
        }

        GLint success = 0;
        glGetShaderiv(handle, GL_COMPILE_STATUS, &success);

        if (success == GL_FALSE) {
            throw RendererException("Shader compilation failed!");
        }
#endif
    }

    return compiled_parts;
}

GLuint compileProgram(FileLoader* loader, const GL3ShaderDefinition& params, const char* header) {
    auto parts = compileShaderParts(loader, params.filenames, header);

    auto prog = glCreateProgram();

    for (auto& part : parts) {
        glAttachShader(prog, part);
    }

    for (auto& attribute : params.attribute_bindings) {
        glBindAttribLocation(prog, attribute.binding_location, attribute.name);
    }

    glLinkProgram(prog);

#ifndef NDEBUG
    GLint logSize = 0;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logSize);

    if (logSize > 1) {
        std::string info_log;
        info_log.resize(logSize);

        glGetProgramInfoLog(prog, logSize, &logSize, &info_log[0]);

        printf("Program info log: %s\n", info_log.c_str());
    }

    GLint success = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &success);

    if (success == GL_FALSE) {
        throw RendererException("Shader linking failed!");
    }
#endif

    for (auto& part : parts) {
        glDetachShader(prog, part);
        glDeleteShader(part);
    }

    return prog;
}

void bindLocations(GLuint program, const GL3ShaderDefinition& def)
{
    for (auto& block : def.buffer_bindings)
    {
        auto blockIndex = glGetUniformBlockIndex(program, block.name);

        if (blockIndex != GL_INVALID_INDEX)
        {
            glUniformBlockBinding(program, blockIndex, block.location);
        }
    }

    glUseProgram(program);
    for (auto& texture : def.texture_bindings) {
        auto uniformIdx = glGetUniformLocation(program, texture.name);
        glUniform1i(uniformIdx, texture.location);
    }
    glUseProgram(0);
}
}

GL3ShaderManager::GL3ShaderManager(FileLoader* fileLoader) : _programCache{0}, _fileLoader(fileLoader) {
}

GL3ShaderManager::~GL3ShaderManager() {
    for (auto prog : _programCache) {
        if (prog != 0) {
            glDeleteProgram(prog);
        }
    }
}

GLuint GL3ShaderManager::getProgram(GL3ShaderType type) {
    if (_programCache[static_cast<size_t>(type)] != 0) {
        return _programCache[static_cast<size_t>(type)];
    }

    auto definition = getShaderDefinition(type);

    auto prog = compileProgram(_fileLoader, definition, "#version 330 core\n#line 1\n");
    bindLocations(prog, definition);
    _programCache[static_cast<size_t>(type)] = prog;

    return _programCache[static_cast<size_t>(type)];
}
void GL3ShaderManager::bindProgram(GL3ShaderType type) {
    GLState->Program.use(getProgram(type));
}
