#pragma once

#include "Enums.hpp"

#include <renderer/ShaderParameters.hpp>
#include <renderer/PipelineState.hpp>

#include <util/Assertion.hpp>

inline GL3ShaderParameterType convertParameterType(ShaderParameterType type) {
    switch (type) {
        case ShaderParameterType::ModelMatrix:
            return GL3ShaderParameterType::ModelMatrix;
        case ShaderParameterType::ViewMatrix:
            return GL3ShaderParameterType::ViewMatrix;
        case ShaderParameterType::ProjectionMatrix:
            return GL3ShaderParameterType::ProjectionMatrix;
        case ShaderParameterType::ColorTexture:
            return GL3ShaderParameterType::ColorTexture;
        case ShaderParameterType::WindowSize:
            return GL3ShaderParameterType::WindowSize;
        case ShaderParameterType::HdrExposure:
            return GL3ShaderParameterType::HdrExposure;
        case ShaderParameterType::BloomHorizontal:
            return GL3ShaderParameterType::BloomHorizontal;
        case ShaderParameterType::BloomedTexture:
            return GL3ShaderParameterType::BloomedTexture;
        default:
            Assertion(false, "Unhandled shader parameter type translation!");
            return GL3ShaderParameterType::ModelMatrix;
    }
}

inline GL3ShaderType convertShaderType(ShaderType type) {
    switch (type) {
        case ShaderType::Mesh:
            return GL3ShaderType::Mesh;
        case ShaderType::LightedMesh:
            return GL3ShaderType::DeferredMesh;
        case ShaderType::PointSprite:
            return GL3ShaderType::PointSprite;
        case ShaderType::InstancedSprite:
            return GL3ShaderType::InstancedSprite;
        case ShaderType::ShadowMesh:
            return GL3ShaderType::ShadowMesh;
        case ShaderType::HdrPostProcessing:
            return GL3ShaderType::HdrPostProcessing;
        case ShaderType::HdrBrightpass:
            return GL3ShaderType::HdrBrightpass;
        case ShaderType::HdrBloom:
            return GL3ShaderType::HdrBloom;
        default:
            Assertion(false, "Unhandled shader type translation!");
            return GL3ShaderType::Mesh;
    }
}

inline Gl3DescriptorSetType convertDescriptorSetType(DescriptorSetType type)
{
    switch(type)
    {
    case DescriptorSetType::ViewSet:
        return Gl3DescriptorSetType::ViewSet;
    case DescriptorSetType::ModelSet:
        return Gl3DescriptorSetType::ModelSet;
    case DescriptorSetType::HdrSet:
        return Gl3DescriptorSetType::HdrSet;
    default: 
        Assertion(false, "Unhandled descriptor set type!");
        return Gl3DescriptorSetType::ViewSet;
    }
}

inline GL3DescriptorSetPart convertDescriptorSetPart(DescriptorSetPart type)
{
    switch (type)
    {
    case DescriptorSetPart::ViewSet_Uniforms:
        return GL3DescriptorSetPart::ViewSet_Uniforms;
    case DescriptorSetPart::ModelSet_Uniforms:
        return GL3DescriptorSetPart::ModelSet_Uniforms;
    case DescriptorSetPart::ModelSet_DiffuseTexture:
        return GL3DescriptorSetPart::ModelSet_DiffuseTexture;
    case DescriptorSetPart::HdrSet_Uniforms:
        return GL3DescriptorSetPart::HdrSet_Uniforms;
    case DescriptorSetPart::HdrSet_BloomedTexture:
        return GL3DescriptorSetPart::HdrSet_BloomedTexture;
    default: 
        Assertion(false, "Unhandled descriptor set part!");
        return GL3DescriptorSetPart::ViewSet_Uniforms;
    }
}

inline GLuint mapDescriptorSetPartLocation(GL3DescriptorSetPart part)
{
    // This is split into uniforms and textures. Currently they are assigned unique indices
    // but that may need to be changed in the future
    switch(part)
    {
    case GL3DescriptorSetPart::ViewSet_Uniforms: 
        return 0;
    case GL3DescriptorSetPart::ModelSet_Uniforms:
        return 1;
    case GL3DescriptorSetPart::HdrSet_Uniforms:
        return 2;
    case GL3DescriptorSetPart::LightingSet_GlobalUniforms:
        return 3;
    case GL3DescriptorSetPart::LightingSet_LightUniforms:
        return 4;

    case GL3DescriptorSetPart::HdrSet_BloomedTexture:
        return 0;
    case GL3DescriptorSetPart::ModelSet_DiffuseTexture:
        return 1;
    case GL3DescriptorSetPart::LightingSet_PositionTexture:
        return 2;
    case GL3DescriptorSetPart::LightingSet_NormalTexture:
        return 3;
    case GL3DescriptorSetPart::LightingSet_AlbedoTexture:
        return 4;
    case GL3DescriptorSetPart::LightingSet_DiretionalShadowMap:
        return 5;

    default:
        Assertion(false, "Unhandled descriptor set part!");
        return 0;
    }
}

inline GLuint mapAttributeLocation(AttributeType type) {
    switch (type) {
        case AttributeType::Position:
            return 0;
        case AttributeType::TexCoord:
            return 1;
        case AttributeType::Normal:
            return 2;
        case AttributeType::Color:
            return 3;
        case AttributeType::Tangent:
            return 4;
        case AttributeType::Bitangent:
            return 5;
        case AttributeType::Radius:
            return 6;
        case AttributeType::PositionOffset:
            return 7;
        default:
            Assertion(false, "Unhandled attribute location mapping!");
            return 0;
    }
}
