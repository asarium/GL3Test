#pragma once

#include "Enums.hpp"
#include <renderer/ShaderParameters.hpp>
#include <renderer/DrawCallManager.hpp>

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
    }
}

inline GL3ShaderType convertShaderType(ShaderType type) {
    switch (type) {
        case ShaderType::Mesh:
            return GL3ShaderType::Mesh;
        case ShaderType::LightedMesh:
            return GL3ShaderType::DeferredMesh;
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
        default:
            return 0;
    }
}
