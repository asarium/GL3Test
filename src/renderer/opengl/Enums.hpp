#pragma once

#include <stddef.h>

template<typename E>
constexpr size_t num_enum_values() {
    return static_cast<size_t>(E::NUM_VALUES);
}

enum class Gl3DescriptorSetType {
    ViewSet,
    ModelSet,
    HdrSet,

    // Lighting pass set
    LightingSet,
    LightSet,

    NanoVGGlobalSet,
    NanoVGLocalSet
};

enum class GL3DescriptorSetPart {
    PushConstantSet_Uniforms,

    ViewSet_Uniforms,

    ModelSet_Uniforms,
    ModelSet_DiffuseTexture,

    HdrSet_BloomedTexture,

    // Lighting pass parts
    LightingSet_PositionTexture,
    LightingSet_NormalTexture,
    LightingSet_AlbedoTexture,

    LightSet_Uniforms,
    LightSet_DirectionalShadowMap,

    // NanoVG parts
    NanoVGGlobalSet_Uniforms,

    NanoVGLocalSet_Uniforms,
    NanoVGLocalSet_Texture
};

enum class GL3ShaderType {
    Mesh = 0,
    DeferredMesh,
    ShadowMesh,
    LightingPass,
    PointSprite,
    InstancedSprite,

    HdrPostProcessing,
    HdrBrightpass,
    HdrBloom,

    NanoVGShader,

    NUM_VALUES
};
