#include "GL3ShaderDefintions.hpp"
#include "EnumTranslation.hpp"

namespace {
struct ShaderDefinition {
  GL3ShaderType type;
  std::vector<ShaderFilename> files;
  std::vector<UniformRequirements> uniform_requirements;
};
ShaderDefinition shader_definitions[] =
    {
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
            },
            {
                {
                    GL3ShaderParameterType::ModelMatrix,
                    { ParameterDataType::Mat4 }
                },
                {
                    GL3ShaderParameterType::ViewMatrix,
                    { ParameterDataType::Mat4 }
                },
                {
                    GL3ShaderParameterType::ProjectionMatrix,
                    { ParameterDataType::Mat4 }
                },
                {
                    GL3ShaderParameterType::ColorTexture,
                    {
                        ParameterDataType::Tex2D,
                        ParameterDataType::Tex2DHandle
                    }
                }
            }
        },
        {
            GL3ShaderType::DeferredMesh,
            {
                {
                    GL_VERTEX_SHADER,
                    "mesh_deferred.vert"
                },
                {
                    GL_FRAGMENT_SHADER,
                    "mesh_deferred.frag"
                }
            },
            {
                {
                    GL3ShaderParameterType::ModelMatrix,
                    { ParameterDataType::Mat4 }
                },
                {
                    GL3ShaderParameterType::ViewMatrix,
                    { ParameterDataType::Mat4 }
                },
                {
                    GL3ShaderParameterType::ProjectionMatrix,
                    { ParameterDataType::Mat4 }
                },
                {
                    GL3ShaderParameterType::ColorTexture,
                    {
                        ParameterDataType::Tex2D,
                        ParameterDataType::Tex2DHandle
                    }
                }
            }
        },
        {
            GL3ShaderType::LightingPass,
            {
                {
                    GL_VERTEX_SHADER,
                    "lighting_pass.vert"
                },
                {
                    GL_FRAGMENT_SHADER,
                    "lighting_pass.frag"
                }
            },
            {
                {
                    GL3ShaderParameterType::ModelMatrix,
                    { ParameterDataType::Mat4 }
                },
                {
                    GL3ShaderParameterType::ViewMatrix,
                    { ParameterDataType::Mat4 }
                },
                {
                    GL3ShaderParameterType::ProjectionMatrix,
                    { ParameterDataType::Mat4 }
                },
                {
                    GL3ShaderParameterType::PositionTexture,
                    {
                        ParameterDataType::Tex2D,
                        ParameterDataType::Tex2DHandle
                    }
                },
                {
                    GL3ShaderParameterType::NormalTexture,
                    {
                        ParameterDataType::Tex2D,
                        ParameterDataType::Tex2DHandle
                    }
                },
                {
                    GL3ShaderParameterType::AlbedoTexture,
                    {
                        ParameterDataType::Tex2D,
                        ParameterDataType::Tex2DHandle
                    }
                },
                {
                    GL3ShaderParameterType::WindowSize,
                    { ParameterDataType::Vec2 }
                },
                {
                    GL3ShaderParameterType::UVScale,
                    { ParameterDataType::Vec2 }
                },
                {
                    GL3ShaderParameterType::LightType,
                    { ParameterDataType::Integer }
                },
                {
                    GL3ShaderParameterType::LightVectorParameter,
                    { ParameterDataType::Vec3 }
                },
                {
                    GL3ShaderParameterType::LightColor,
                    { ParameterDataType::Vec3 }
                },
                {
                    GL3ShaderParameterType::LightIntensitiy,
                    { ParameterDataType::Float }
                }
            }
        },
        {
            GL3ShaderType::PointSprite,
            {
                {
                    GL_VERTEX_SHADER,
                    "point_sprite.vert"
                },
                {
                    GL_GEOMETRY_SHADER,
                    "point_sprite.geom"
                },
                {
                    GL_FRAGMENT_SHADER,
                    "2d_sprite.frag"
                }
            },
            {
                {
                    GL3ShaderParameterType::ViewMatrix,
                    { ParameterDataType::Mat4 }
                },
                {
                    GL3ShaderParameterType::ProjectionMatrix,
                    { ParameterDataType::Mat4 }
                },
                {
                    GL3ShaderParameterType::ColorTexture,
                    {
                        ParameterDataType::Tex2D,
                        ParameterDataType::Tex2DHandle
                    }
                }
            }
        },
        {
            GL3ShaderType::InstancedSprite,
            {
                {
                    GL_VERTEX_SHADER,
                    "2d_sprite.vert"
                },
                {
                    GL_FRAGMENT_SHADER,
                    "2d_sprite.frag"
                }
            },
            {
                {
                    GL3ShaderParameterType::ViewMatrix,
                    { ParameterDataType::Mat4 }
                },
                {
                    GL3ShaderParameterType::ProjectionMatrix,
                    { ParameterDataType::Mat4 }
                },
                {
                    GL3ShaderParameterType::ColorTexture,
                    {
                        ParameterDataType::Tex2D,
                        ParameterDataType::Tex2DHandle
                    }
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
    },
    {
        GL3ShaderParameterType::UVScale,
        "uv_scale"
    },
    {
        GL3ShaderParameterType::PositionTexture,
        "g_position"
    },
    {
        GL3ShaderParameterType::NormalTexture,
        "g_normal"
    },
    {
        GL3ShaderParameterType::AlbedoTexture,
        "g_albedo"
    },
    {
        GL3ShaderParameterType::LightType,
        "light_type"
    },
    {
        GL3ShaderParameterType::LightVectorParameter,
        "light_vector"
    },
    {
        GL3ShaderParameterType::LightColor,
        "light_color"
    },
    {
        GL3ShaderParameterType::LightIntensitiy,
        "light_intensity"
    }
};

AttributeBinding attribute_mappings[] =
    {
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
        },
        {
            AttributeType::Tangent,
            "in_tangent",
            mapAttributeLocation(AttributeType::Tangent)
        },
        {
            AttributeType::Radius,
            "in_radius",
            mapAttributeLocation(AttributeType::Radius)
        },
        {
            AttributeType::PositionOffset,
            "in_pos_offset",
            mapAttributeLocation(AttributeType::PositionOffset)
        }
    };
}

GL3ShaderDefinition getShaderDefinition(GL3ShaderType type) {
    GL3ShaderDefinition def = GL3ShaderDefinition();

    for (auto& file:shader_definitions) {
        if (file.type == type) {
            def.filenames = file.files;
            def.uniform_requirements = file.uniform_requirements;
        }
    }

    for (auto& attr : attribute_mappings) {
        def.attribute_bindings.push_back(attr);
    }

    for (auto& unif : uniform_mappings) {
        def.uniforms.push_back(unif);
    }

    return def;
}

std::vector<GL3ShaderType> getDefinedShaderTypes() {
    std::vector<GL3ShaderType> types;
    for (auto& file:shader_definitions) {
        types.push_back(file.type);
    }
    return types;
}
