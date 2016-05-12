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
};

enum class GL3DescriptorSetPart {
    PushConstantSet_Uniforms,

    ViewSet_Uniforms,

    ModelSet_Uniforms,
    ModelSet_DiffuseTexture,

    HdrSet_BloomedTexture,

    // Lighting pass parts
    LightingSet_GlobalUniforms,
    LightingSet_LightUniforms,
    LightingSet_PositionTexture,
    LightingSet_NormalTexture,
    LightingSet_AlbedoTexture,
    LightingSet_DiretionalShadowMap,
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

    NUM_VALUES
};
