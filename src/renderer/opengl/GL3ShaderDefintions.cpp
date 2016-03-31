#include "GL3ShaderDefintions.hpp"
#include "EnumTranslation.hpp"

namespace {
    struct ShaderDefinition {
        GL3ShaderType type;
        std::vector<ShaderFilename> files;
    };
    ShaderDefinition shader_definitions[] = {
        {
            GL3ShaderType::Mesh,
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

    UniformMapping uniform_mappings[] = {
        {
            GL3ShaderParameterType::ViewMatrix,
            "view_matrix"
        },
        {
            GL3ShaderParameterType::ModelMatrix,
            "model_matrix"
        },
        {
            GL3ShaderParameterType::ProjectionMatrix,
            "proj_matrix"
        },
        {
            GL3ShaderParameterType::ColorTexture,
            "color_texture"
        },
        {
            GL3ShaderParameterType::WindowSize,
            "window_size"
        }
    };

    AttributeBinding attribute_mappings[] = {
        {
            AttributeType::Position,
            "in_position",
            mapAttributeLocation(AttributeType::Position)
        },
        {
            AttributeType::Color,
            "in_color",
            mapAttributeLocation(AttributeType::Color)
        },
        {
            AttributeType::Normal,
            "in_normal",
            mapAttributeLocation(AttributeType::Normal)
        },
        {
            AttributeType::TexCoord,
            "in_tex_coord",
            mapAttributeLocation(AttributeType::TexCoord)
        }
    };
}

GL3ShaderDefinition getShaderDefinition(GL3ShaderType type) {
    GL3ShaderDefinition def = GL3ShaderDefinition();

    for (auto &file:shader_definitions) {
        if (file.type == type)
            def.filenames = file.files;
    }

    for (auto &attr : attribute_mappings) {
        def.attribute_bindings.push_back(attr);
    }

    for (auto &unif : uniform_mappings) {
        def.uniforms.push_back(unif);
    }

    return def;
}
