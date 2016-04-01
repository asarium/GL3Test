//
//

#include "GL3ShaderProgram.hpp"
#include "GL3VertexLayout.hpp"
#include "GL3ShaderParameters.hpp"
#include "GL3ShaderDefintions.hpp"
#include "GL3State.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <renderer/VertexLayout.hpp>

namespace {
    std::vector<GLuint> compileShaderParts(FileLoader *loader, const std::vector<ShaderFilename> &parts) {
        std::vector<GLuint> compiled_parts;
        compiled_parts.reserve(parts.size());

        for (auto &filename : parts) {
            auto handle = glCreateShader(filename.type);

            auto content = loader->getFileContents(filename.filename);
            const GLchar *contentStr = reinterpret_cast<GLchar *>(content.data());
            GLint length = static_cast<GLint>(content.size());

            glShaderSource(handle, 1, &contentStr, &length);

            glCompileShader(handle);

            compiled_parts.push_back(handle);
#ifndef NDEBUG
            GLint success = 0;
            glGetShaderiv(handle, GL_COMPILE_STATUS, &success);

            if (success == GL_FALSE) {
                printf("Shader compilation failed!\n");
                GLint logSize = 0;
                glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logSize);

                if (logSize > 0) {
                    std::string info_log;
                    info_log.resize(logSize);

                    glGetShaderInfoLog(handle, logSize, &logSize, &info_log[0]);

                    printf("Shader info log: %s\n", info_log.c_str());
                }
            }
#endif
        }

        return compiled_parts;
    }

    GLuint compileProgram(FileLoader *loader, const GL3ShaderDefinition &params) {
        auto parts = compileShaderParts(loader, params.filenames);

        auto prog = glCreateProgram();

        for (auto &part : parts) {
            glAttachShader(prog, part);
        }

        for (auto &attribute : params.attribute_bindings) {
            glBindAttribLocation(prog, attribute.binding_location, attribute.name);
        }

        glLinkProgram(prog);

#ifndef NDEBUG
        GLint success = 0;
        glGetProgramiv(prog, GL_LINK_STATUS, &success);

        if (success == GL_FALSE) {
            printf("Shader linking failed!\n");

            GLint logSize = 0;
            glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logSize);

            if (logSize > 0) {
                std::string info_log;
                info_log.resize(logSize);

                glGetProgramInfoLog(prog, logSize, &logSize, &info_log[0]);

                printf("Program info log: %s\n", info_log.c_str());
            }
        }
#endif

        for (auto &part : parts) {
            glDetachShader(prog, part);
            glDeleteShader(part);
        }

        return prog;
    }
}

GL3ShaderProgram::GL3ShaderProgram(FileLoader *loader, const GL3ShaderDefinition &definition) {
    _handle = compileProgram(loader, definition);

    // Now get the locations of the uniforms
    for (auto &mapping : definition.uniforms) {
        auto loc = glGetUniformLocation(_handle, mapping.name);

        _uniformLocations[static_cast<size_t>(mapping.parameter)] = loc;
    }
}

GL3ShaderProgram::~GL3ShaderProgram() {
    glDeleteProgram(_handle);
}

void GL3ShaderProgram::bind() {
    GLState->Program.use(_handle);
}

void GL3ShaderProgram::bindAndSetParameters(const GL3ShaderParameters *parameters) {
    this->bind();
    GLint texture_unit = 0;

    GLState->Texture.unbindAll();

    for (auto &parameter : parameters->getValues()) {
        auto uniform_loc = getUniformLocation(parameter.param_type);

        if (uniform_loc < 0) {
            // Uniform isn't used in this shader
            continue;
        }

        switch (parameter.data_type) {
            case ParameterDataType::Vec2:
                glUniform2fv(uniform_loc, 1, glm::value_ptr(parameter.value.vec2));
                break;
            case ParameterDataType::Vec3:
                glUniform3fv(uniform_loc, 1, glm::value_ptr(parameter.value.vec3));
                break;
            case ParameterDataType::Mat4:
                glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(parameter.value.mat4));
                break;
            case ParameterDataType::Tex2D:
                parameter.value.tex2d->bind(texture_unit);

                glUniform1i(uniform_loc, texture_unit);

                ++texture_unit;
                break;
            case ParameterDataType::Tex2DHandle:
                GLState->Texture.bindTexture(texture_unit, GL_TEXTURE_2D, parameter.value.tex2dhandle);

                glUniform1i(uniform_loc, texture_unit);
                
                ++texture_unit;
                break;
            case ParameterDataType::Integer:
                glUniform1i(uniform_loc, parameter.value.integer);
                break;
            case ParameterDataType::Float:
                glUniform1f(uniform_loc, parameter.value.floatVal);
                break;
        }
    }
}

