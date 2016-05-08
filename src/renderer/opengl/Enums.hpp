#pragma once

#include <stddef.h>

template<typename E>
constexpr size_t num_enum_values() {
    return static_cast<size_t>(E::NUM_VALUES);
}

// This is basically the same as ShaderParameterType but enums can't inherit values from another so they have to be duplicated
enum class GL3ShaderParameterType {
    ModelMatrix = 0,
    ViewMatrix,
    ProjectionMatrix,
    ColorTexture,
    WindowSize,

    UVScale,

    // Parameters for the deferred lighting shader
    PositionTexture,
    NormalTexture,
    AlbedoTexture,

    LightType,
    LightVectorParameter,
    LightColor,
    LightIntensitiy,

    LightHasShadow,
    DirectionalShadowMap,
    LightProjectionMatrix,
    LightViewMatrix,

    // HDR post processing
    HdrExposure,
    BloomHorizontal,
    BloomedTexture,

    NUM_VALUES
};

enum class Gl3DescriptorSetType {
    ViewSet,
    ModelSet,
    HdrSet,

    // Lighting pass set
    LightingSet,
};

enum class GL3DescriptorSetPart {
    ViewSet_Uniforms,

    ModelSet_Uniforms,
    ModelSet_DiffuseTexture,

    HdrSet_Uniforms,
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
