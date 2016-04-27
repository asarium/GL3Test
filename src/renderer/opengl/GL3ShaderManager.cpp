//
//

#include "GL3ShaderManager.hpp"

namespace {
std::vector<GLuint> compileShaderParts(FileLoader* loader, const std::vector<ShaderFilename>& parts) {
    std::vector<GLuint> compiled_parts;
    compiled_parts.reserve(parts.size());

    for (auto& filename : parts) {
        printf("Compiling %s...\n", filename.filename);
        auto handle = glCreateShader(filename.type);

        auto content = loader->getFileContents(filename.filename);
        const GLchar* contentStr = reinterpret_cast<GLchar*>(content.data());
        GLint length = static_cast<GLint>(content.size());

        glShaderSource(handle, 1, &contentStr, &length);

        glCompileShader(handle);

        compiled_parts.push_back(handle);
#ifndef NDEBUG
        GLint success = 0;
        glGetShaderiv(handle, GL_COMPILE_STATUS, &success);

        if (success == GL_FALSE) {
            printf("Shader compilation failed!\n");
        }

        GLint logSize = 0;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logSize);

        if (logSize > 1) {
            std::string info_log;
            info_log.resize(logSize);

            glGetShaderInfoLog(handle, logSize, &logSize, &info_log[0]);

            printf("Shader info log: %s\n", info_log.c_str());
        }
#endif
    }

    return compiled_parts;
}

GLuint compileProgram(FileLoader* loader, const GL3ShaderDefinition& params) {
    auto parts = compileShaderParts(loader, params.filenames);

    auto prog = glCreateProgram();

    for (auto& part : parts) {
        glAttachShader(prog, part);
    }

    for (auto& attribute : params.attribute_bindings) {
        glBindAttribLocation(prog, attribute.binding_location, attribute.name);
    }

    glLinkProgram(prog);

#ifndef NDEBUG
    GLint success = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &success);

    if (success == GL_FALSE) {
        printf("Shader linking failed!\n");
    }

    GLint logSize = 0;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logSize);

    if (logSize > 1) {
        std::string info_log;
        info_log.resize(logSize);

        glGetProgramInfoLog(prog, logSize, &logSize, &info_log[0]);

        printf("Program info log: %s\n", info_log.c_str());
    }
#endif

    for (auto& part : parts) {
        glDetachShader(prog, part);
        glDeleteShader(part);
    }

    return prog;
}
}

GL3ShaderManager::GL3ShaderManager(FileLoader* fileLoader) : _fileLoader(fileLoader) {
}

GL3ShaderManager::~GL3ShaderManager() {
}

GL3ShaderProgram* GL3ShaderManager::getShader(GL3ShaderType type) {
    if (_programCache[static_cast<size_t>(type)]) {
        return _programCache[static_cast<size_t>(type)].get();
    }

    auto definition = getShaderDefinition(type);
    _programCache[static_cast<size_t>(type)].reset(new GL3ShaderProgram(compileProgram(_fileLoader, definition),
                                                                        definition));

    return _programCache[static_cast<size_t>(type)].get();
}

