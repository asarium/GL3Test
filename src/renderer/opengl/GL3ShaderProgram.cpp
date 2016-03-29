//
//

#include "GL3ShaderProgram.hpp"
#include "GL3VertexLayout.hpp"
#include "GL3ShaderParameters.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <renderer/VertexLayout.hpp>

namespace {
    struct ShaderFilename {
        GLenum type;
        const char *filename;
    };

    struct ShaderDefinition {
        ShaderType type;
        std::vector<ShaderFilename> files;
    };
    ShaderDefinition shader_definitions[] = {
            {
                    ShaderType::Model,
                    {
                            {
                                    GL_VERTEX_SHADER,
                                    "model.vert"
                            },
                            {
                                    GL_FRAGMENT_SHADER,
                                    "model.frag"
                            }
                    }
            }
    };

    struct UniformNameMapping {
        ShaderParameterType parameter;
        const GLchar *uniform_name;
    };
    UniformNameMapping uniform_mappings[] = {
            {
                    ShaderParameterType::ViewMatrix,
                    "view_matrix"
            },
            {
                    ShaderParameterType::ModelMatrix,
                    "model_matrix"
            },
            {
                    ShaderParameterType::ProjectionMatrix,
                    "proj_matrix"
            },
            {
                    ShaderParameterType::ColorTexture,
                    "color_texture"
            },
            {
                    ShaderParameterType::WindowSize,
                    "window_size"
            }
    };
    static_assert((sizeof(uniform_mappings)/sizeof(uniform_mappings[0])) == NUM_SHADER_PARAMETER,
                  "NUM_SHADER_PARAMETER and defined uniforms does not match!");

    struct AttributeNameMapping {
        AttributeType attribute;
        const GLchar *name;
    };
    AttributeNameMapping attribute_mappings[] = {
            {
                    AttributeType::Position,
                    "in_position"
            },
            {
                    AttributeType::Color,
                    "in_color"
            },
            {
                    AttributeType::Normal,
                    "in_normal"
            },
            {
                    AttributeType::TexCoord,
                    "in_tex_coord"
            }
    };

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

    GLuint compileProgram(FileLoader *loader, const ShaderDefinition &params) {
        auto parts = compileShaderParts(loader, params.files);

        auto prog = glCreateProgram();

        for (auto &part : parts) {
            glAttachShader(prog, part);
        }

        for (auto &attribute : attribute_mappings) {
            glBindAttribLocation(prog, GL3VertexLayout::mapAttributeLocation(attribute.attribute), attribute.name);
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

GL3ShaderProgram::GL3ShaderProgram(FileLoader *loader, ShaderType type) {
    for (auto &def : shader_definitions) {
        if (def.type == type) {
            _handle = compileProgram(loader, def);
            break;
        }
    }

    // Now get the locations of the uniforms
    for (auto &mapping : uniform_mappings) {
        auto loc = glGetUniformLocation(_handle, mapping.uniform_name);

        _uniformLocations[getParameterIndex(mapping.parameter)] = loc;
    }
}

GL3ShaderProgram::~GL3ShaderProgram() {
    glDeleteProgram(_handle);
}

void GL3ShaderProgram::bind() {
    glUseProgram(_handle);
}

GLint GL3ShaderProgram::getUniformLocation(ShaderParameterType param) {
    return _uniformLocations[getParameterIndex(param)];
}

void GL3ShaderProgram::bindAndSetParameters(const GL3ShaderParameters *parameters) {
    this->bind();
    GLint texture_unit = 0;

    for (auto &parameter : parameters->getValues()) {
        auto uniform_loc = getUniformLocation(parameter.param_type);

        switch (parameter.data_type) {
            case ParameterDataType::Vec2:
                glUniform2fv(uniform_loc, 1, glm::value_ptr(parameter.value.vec2));
                break;
            case ParameterDataType::Mat4:
                glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(parameter.value.mat4));
                break;
            case ParameterDataType::Tex2D:
                glActiveTexture(GL_TEXTURE0 + texture_unit);
                parameter.value.tex2d->bind();

                glUniform1i(uniform_loc, texture_unit);

                ++texture_unit;
                break;
        }
    }
}

size_t GL3ShaderProgram::getParameterIndex(ShaderParameterType type) {
    switch (type) {
        case ShaderParameterType::ModelMatrix:
            return 0;
        case ShaderParameterType::ViewMatrix:
            return 1;
        case ShaderParameterType::ProjectionMatrix:
            return 2;
        case ShaderParameterType::ColorTexture:
            return 3;
        case ShaderParameterType::WindowSize:
            return 4;
    }
    return 0;
}

