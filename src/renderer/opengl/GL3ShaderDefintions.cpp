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
        },
        {
            GL3ShaderType::NanoVGShader,
            {
                {
                    GL_VERTEX_SHADER,
                    "nanovg.vert",
                },
                {
                    GL_FRAGMENT_SHADER,
                    "nanovg.frag",
                }
            }
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
        },
        {
            AttributeType::Position2D,
            "in_position_2d",
            mapAttributeLocation(AttributeType::Position2D)
        }
    };
}

DescriptorBinding uniform_buffer_bindings[] = {
    {
        GL3DescriptorSetPart::PushConstantSet_Uniforms,
        "PushConstants",
        mapDescriptorSetPartLocation(GL3DescriptorSetPart::PushConstantSet_Uniforms)
    },
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
        GL3DescriptorSetPart::LightSet_Uniforms,
        "LightData",
        mapDescriptorSetPartLocation(GL3DescriptorSetPart::LightSet_Uniforms)
    },
    {
        GL3DescriptorSetPart::NanoVGGlobalSet_Uniforms,
        "NanoVGGlobalData",
        mapDescriptorSetPartLocation(GL3DescriptorSetPart::NanoVGGlobalSet_Uniforms)
    },
    {
        GL3DescriptorSetPart::NanoVGLocalSet_Uniforms,
        "NanoVGUniformData",
        mapDescriptorSetPartLocation(GL3DescriptorSetPart::NanoVGLocalSet_Uniforms)
    }
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
        GL3DescriptorSetPart::LightSet_DirectionalShadowMap,
        "directional_shadow_map",
        mapDescriptorSetPartLocation(GL3DescriptorSetPart::LightSet_DirectionalShadowMap)
    },
    {
        GL3DescriptorSetPart::NanoVGLocalSet_Texture,
        "nvg_tex",
        mapDescriptorSetPartLocation(GL3DescriptorSetPart::NanoVGLocalSet_Texture)
    }
};

GL3ShaderDefinition getShaderDefinition(GL3ShaderType type) {
    GL3ShaderDefinition def;

    for (auto& file:shader_definitions) {
        if (file.type == type) {
            def.filenames = file.files;
            break;
        }
    }

    for (auto& attr : attribute_mappings) {
        def.attribute_bindings.push_back(attr);
    }

    for (auto& buffer : uniform_buffer_bindings) {
        def.buffer_bindings.push_back(buffer);
    }

    for (auto& buffer : texture_bindings) {
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
