#include "GL3ShaderDefintions.hpp"
#include "EnumTranslation.hpp"

namespace {
struct ShaderDefinition {
  GL3ShaderType type;
  std::vector<ShaderFilename> files;
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
            }
        },
        {
            GL3ShaderType::ShadowMesh,
            {
                {
                    GL_VERTEX_SHADER,
                    "mesh_shadow.vert"
                },
                {
                    GL_FRAGMENT_SHADER,
                    "mesh_shadow.frag"
                }
            }
        },
        {
            GL3ShaderType::HdrPostProcessing,
            {
                {
                    GL_VERTEX_SHADER,
                    "direct_passthrough.vert",
                },
                {
                    GL_FRAGMENT_SHADER,
                    "hdr_post_processing.frag",
                }
            }
        },
        {
            GL3ShaderType::HdrBrightpass,
            {
                {
                    GL_VERTEX_SHADER,
                    "direct_passthrough.vert",
                },
                {
                    GL_FRAGMENT_SHADER,
                    "brightpass.frag",
                }
            }
        },
        {
            GL3ShaderType::HdrBloom,
            {
                {
                    GL_VERTEX_SHADER,
                    "direct_passthrough.vert",
                },
                {
                    GL_FRAGMENT_SHADER,
                    "bloom_pass.frag",
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
    },
    {
        GL3ShaderParameterType::LightHasShadow,
        "light_has_shadow"
    },
    {
        GL3ShaderParameterType::DirectionalShadowMap,
        "directional_shadow_map"
    },
    {
        GL3ShaderParameterType::LightProjectionMatrix,
        "light_proj_matrix"
    },
    {
        GL3ShaderParameterType::LightViewMatrix,
        "light_view_matrix"
    },
    {
        GL3ShaderParameterType::HdrExposure,
        "hdr_exposure"
    },
    {
        GL3ShaderParameterType::BloomHorizontal,
        "horizontal"
    },
    {
        GL3ShaderParameterType::BloomedTexture,
        "bloomed_texture"
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

DescriptorBinding uniform_buffer_bindings[] = {
    {
        GL3DescriptorSetPart::ViewSet_Uniforms,
        "ViewData",
        mapDescriptorSetPartLocation(GL3DescriptorSetPart::ViewSet_Uniforms)
    },
    {
        GL3DescriptorSetPart::ModelSet_Uniforms,
        "ModelData",
        mapDescriptorSetPartLocation(GL3DescriptorSetPart::ModelSet_Uniforms)
    },
    {
        GL3DescriptorSetPart::HdrSet_Uniforms,
        "HDRData",
        mapDescriptorSetPartLocation(GL3DescriptorSetPart::HdrSet_Uniforms)
    },
    {
        GL3DescriptorSetPart::LightingSet_GlobalUniforms,
        "GlobalLightingData",
        mapDescriptorSetPartLocation(GL3DescriptorSetPart::LightingSet_GlobalUniforms)
    },
    {
        GL3DescriptorSetPart::LightingSet_LightUniforms,
        "LightData",
        mapDescriptorSetPartLocation(GL3DescriptorSetPart::LightingSet_LightUniforms)
    },
};

DescriptorBinding texture_bindings[] = {
    {
        GL3DescriptorSetPart::ModelSet_DiffuseTexture,
        "color_texture",
        mapDescriptorSetPartLocation(GL3DescriptorSetPart::ModelSet_DiffuseTexture)
    },
    {
        GL3DescriptorSetPart::HdrSet_BloomedTexture,
        "bloomed_texture",
        mapDescriptorSetPartLocation(GL3DescriptorSetPart::HdrSet_BloomedTexture)
    },
    {
        GL3DescriptorSetPart::LightingSet_PositionTexture,
        "g_position",
        mapDescriptorSetPartLocation(GL3DescriptorSetPart::LightingSet_PositionTexture)
    },
    {
        GL3DescriptorSetPart::LightingSet_NormalTexture,
        "g_normal",
        mapDescriptorSetPartLocation(GL3DescriptorSetPart::LightingSet_NormalTexture)
    },
    {
        GL3DescriptorSetPart::LightingSet_AlbedoTexture,
        "g_albedo",
        mapDescriptorSetPartLocation(GL3DescriptorSetPart::LightingSet_AlbedoTexture)
    },
    {
        GL3DescriptorSetPart::LightingSet_DiretionalShadowMap,
        "directional_shadow_map",
        mapDescriptorSetPartLocation(GL3DescriptorSetPart::LightingSet_DiretionalShadowMap)
    },
};

GL3ShaderDefinition getShaderDefinition(GL3ShaderType type) {
    GL3ShaderDefinition def;

    for (auto& file:shader_definitions) {
        if (file.type == type) {
            def.filenames = file.files;
        }
    }

    for (auto& attr : attribute_mappings) {
        def.attribute_bindings.push_back(attr);
    }

    for (auto& unif : uniform_mappings) {
        def.uniforms.push_back(unif);
    }

    for (auto& buffer : uniform_buffer_bindings)
    {
        def.buffer_bindings.push_back(buffer);
    }

    for (auto& buffer : texture_bindings)
    {
        def.texture_bindings.push_back(buffer);
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
