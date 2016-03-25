//
//

#include "GL3ShaderProgram.hpp"
#include "GL3VertexLayout.hpp"

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
                    .type = ShaderType::Model,
                    .files = {
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
        ShaderParameter parameter;
        const GLchar *uniform_name;
    };
    UniformNameMapping uniform_mappings[] = {
            {
                    .parameter = ShaderParameter::ViewMatrix,
                    .uniform_name = "view_matrix"
            },
            {
                    .parameter = ShaderParameter::ModelMatrix,
                    .uniform_name = "model_matrix"
            },
            {
                    .parameter=ShaderParameter::ProjectionMatrix,
                    .uniform_name = "proj_matrix"
            }
    };

    struct AttributeNameMapping {
        AttributeType attribute;
        const GLchar *name;
    };
    AttributeNameMapping attribute_mappings[] = {
            {
                    .attribute = AttributeType::Position,
                    .name = "in_position"
            },
            {
                    .attribute = AttributeType::Color,
                    .name = "in_color"
            },
            {
                    .attribute = AttributeType::Normal,
                    .name = "in_normal"
            },
            {
                    .attribute = AttributeType::TexCoord,
                    .name = "in_tex_coord"
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

        // TODO: Bind attribute locations here
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

        ParameterLocation param_loc;
        param_loc.location = loc;
        param_loc.param = mapping.parameter;
        _uniformLocations.push_back(param_loc);
    }
}

GL3ShaderProgram::~GL3ShaderProgram() {
    glDeleteProgram(_handle);
}

void GL3ShaderProgram::setParameterMat4(ShaderParameter param, const glm::mat4 &value) {
    this->bind();

    auto uniform_loc = getUniformLocation(param);
    glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(value));
}

void GL3ShaderProgram::bind() {
    glUseProgram(_handle);
}

GLint GL3ShaderProgram::getUniformLocation(ShaderParameter param) {
    for (auto &loc:_uniformLocations) {
        if (loc.param == param) {
            return loc.location;
        }
    }
    return -1;
}

