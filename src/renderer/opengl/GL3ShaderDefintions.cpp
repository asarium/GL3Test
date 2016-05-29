#include "GL3ShaderDefintions.hpp"
#include "EnumTranslation.hpp"

namespace {
struct ShaderDefinition {
    ShaderType type;
    std::vector<ShaderFilename> files;
    std::vector<ShaderFlags> supportedFlags;
};
ShaderDefinition shader_definitions[] =
    {
        {
            ShaderType::Mesh,
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
            }
        },
        {
            ShaderType::LightedMesh,
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
            }
        },
        {
            ShaderType::LightingPass,
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
            }
        },
        {
            ShaderType::PointSprite,
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
            }
        },
        {
            ShaderType::InstancedSprite,
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
            }
        },
        {
            ShaderType::ShadowMesh,
            {
                {
                    GL_VERTEX_SHADER,
                    "mesh_shadow.vert"
                },
                {
                    GL_FRAGMENT_SHADER,
                    "mesh_shadow.frag"
                }
            },
            {
            }
        },
        {
            ShaderType::HdrPostProcessing,
            {
                {
                    GL_VERTEX_SHADER,
                    "direct_passthrough.vert",
                },
                {
                    GL_FRAGMENT_SHADER,
                    "hdr_post_processing.frag",
                }
            },
            {
            }
        },
        {
            ShaderType::HdrBrightpass,
            {
                {
                    GL_VERTEX_SHADER,
                    "direct_passthrough.vert",
                },
                {
                    GL_FRAGMENT_SHADER,
                    "brightpass.frag",
                }
            },
            {
            }
        },
        {
            ShaderType::HdrBloom,
            {
                {
                    GL_VERTEX_SHADER,
                    "direct_passthrough.vert",
                },
                {
                    GL_FRAGMENT_SHADER,
                    "bloom_pass.frag",
                }
            },
            {
            }
        },
        {
            ShaderType::NanoVGShader,
            {
                {
                    GL_VERTEX_SHADER,
                    "nanovg.vert",
                },
                {
                    GL_FRAGMENT_SHADER,
                    "nanovg.frag",
                }
            },
            {
                ShaderFlags::NanoVGEdgeAA
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

// The following is from http://stackoverflow.com/a/9253062/844001
// Returns which bits are on in the integer a
std::vector<size_t> getOnLocations(size_t a) {
    std::vector<size_t> result;
    size_t place = 0;
    while (a != 0) {
        if (a & 1) {
            result.push_back(place);
        }
        ++place;
        a >>= 1;
    }
    return result;
}

template<typename T>
std::vector<std::vector<T> > powerSet(const std::vector<T>& set) {
    std::vector<std::vector<T> > result;
    size_t numPowerSets = static_cast<size_t>(pow(2.0, static_cast<double>(set.size())));
    for (size_t i = 0; i < numPowerSets; ++i) {
        std::vector<size_t> onLocations = getOnLocations(i);
        std::vector<T> subSet;
        for (size_t j = 0; j < onLocations.size(); ++j) {
            subSet.push_back(set.at(onLocations.at(j)));
        }
        result.push_back(subSet);
    }
    return result;
}

ShaderFlags combineShaderFlags(const std::vector<ShaderFlags>& flags) {
    ShaderFlags ret = ShaderFlags::None;
    for (auto flag : flags) {
        ret |= flag;
    }
    return ret;
}
}

GL3ShaderDefinition getShaderDefinition(ShaderType type) {
    GL3ShaderDefinition def;

    ShaderDefinition shaderDef;
    for (auto& file:shader_definitions) {
        if (file.type == type) {
            shaderDef = file;
            break;
        }
    }
    def.filenames = shaderDef.files;

    def.allFlags = combineShaderFlags(shaderDef.supportedFlags);

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

std::vector<std::pair<ShaderType, ShaderFlags>> getDefinedShaderTypes() {
    std::vector<std::pair<ShaderType, ShaderFlags>> types;
    for (auto& file:shader_definitions) {
        auto combinations = powerSet(file.supportedFlags);
        for (auto& combs : combinations) {
            types.push_back(std::make_pair(file.type, combineShaderFlags(combs)));
        }
    }
    return types;
}
