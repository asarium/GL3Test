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

inline GLint mapAttributeLocation(AttributeType type) {
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
