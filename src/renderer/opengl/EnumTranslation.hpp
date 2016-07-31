#pragma once

#include "Enums.hpp"

#include <renderer/PipelineState.hpp>
#include <renderer/VertexLayout.hpp>
#include <renderer/ShaderParameters.hpp>

#include <util/Assertion.hpp>

inline Gl3DescriptorSetType convertDescriptorSetType(DescriptorSetType type) {
    switch (type) {
        case DescriptorSetType::ViewSet:
            return Gl3DescriptorSetType::ViewSet;
        case DescriptorSetType::ModelSet:
            return Gl3DescriptorSetType::ModelSet;
        case DescriptorSetType::HdrSet:
            return Gl3DescriptorSetType::HdrSet;
        case DescriptorSetType::LightingSet:
            return Gl3DescriptorSetType::LightingSet;
        case DescriptorSetType::LightSet:
            return Gl3DescriptorSetType::LightSet;
        case DescriptorSetType::NanoVGGlobalSet:
            return Gl3DescriptorSetType::NanoVGGlobalSet;
        case DescriptorSetType::NanoVGLocalSet:
            return Gl3DescriptorSetType::NanoVGLocalSet;
        default:
            Assertion(false, "Unhandled descriptor set type!");
            return Gl3DescriptorSetType::ViewSet;
    }
}

inline GL3DescriptorSetPart convertDescriptorSetPart(DescriptorSetPart type) {
    switch (type) {
        case DescriptorSetPart::ViewSet_Uniforms:
            return GL3DescriptorSetPart::ViewSet_Uniforms;
        case DescriptorSetPart::ModelSet_Uniforms:
            return GL3DescriptorSetPart::ModelSet_Uniforms;
        case DescriptorSetPart::ModelSet_DiffuseTexture:
            return GL3DescriptorSetPart::ModelSet_DiffuseTexture;
        case DescriptorSetPart::HdrSet_BloomedTexture:
            return GL3DescriptorSetPart::HdrSet_BloomedTexture;
        case DescriptorSetPart::LightingSet_PositionTexture:
            return GL3DescriptorSetPart::LightingSet_PositionTexture;
        case DescriptorSetPart::LightingSet_NormalTexture:
            return GL3DescriptorSetPart::LightingSet_NormalTexture;
        case DescriptorSetPart::LightingSet_AlbedoTexture:
            return GL3DescriptorSetPart::LightingSet_AlbedoTexture;
        case DescriptorSetPart::LightSet_Uniforms:
            return GL3DescriptorSetPart::LightSet_Uniforms;
        case DescriptorSetPart::LightSet_DirectionalShadowMap:
            return GL3DescriptorSetPart::LightSet_DirectionalShadowMap;
        case DescriptorSetPart::NanoVGGlobalSet_Uniforms:
            return GL3DescriptorSetPart::NanoVGGlobalSet_Uniforms;
        case DescriptorSetPart::NanoVGLocalSet_Uniforms:
            return GL3DescriptorSetPart::NanoVGLocalSet_Uniforms;
        case DescriptorSetPart::NanoVGLocalSet_Texture:
            return GL3DescriptorSetPart::NanoVGLocalSet_Texture;
                
        default:
            Assertion(false, "Unhandled descriptor set part!");
            return GL3DescriptorSetPart::ViewSet_Uniforms;
    }
}

inline GLuint mapDescriptorSetPartLocation(GL3DescriptorSetPart part) {
    // This is split into uniforms and textures. Currently they are assigned unique indices
    // but that may need to be changed in the future
    switch (part) {
        case GL3DescriptorSetPart::PushConstantSet_Uniforms:
            return 0;
        case GL3DescriptorSetPart::ViewSet_Uniforms:
            return 1;
        case GL3DescriptorSetPart::ModelSet_Uniforms:
            return 2;
        case GL3DescriptorSetPart::LightSet_Uniforms:
            return 3;
        case GL3DescriptorSetPart::NanoVGGlobalSet_Uniforms:
            return 4;
        case GL3DescriptorSetPart::NanoVGLocalSet_Uniforms:
            return 5;

        case GL3DescriptorSetPart::HdrSet_BloomedTexture:
            return 1;
        case GL3DescriptorSetPart::ModelSet_DiffuseTexture:
            return 2;
        case GL3DescriptorSetPart::LightingSet_PositionTexture:
            return 3;
        case GL3DescriptorSetPart::LightingSet_NormalTexture:
            return 4;
        case GL3DescriptorSetPart::LightingSet_AlbedoTexture:
            return 5;
        case GL3DescriptorSetPart::LightSet_DirectionalShadowMap:
            return 6;
        case GL3DescriptorSetPart::NanoVGLocalSet_Texture:
            return 7;

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
        case AttributeType::Position2D:
            return 8;
        default:
            Assertion(false, "Unhandled attribute location mapping!");
            return 0;
    }
}

inline GLenum convertComparisionFunction(ComparisionFunction func) {
    GLenum mode;
    switch (func) {
        case ComparisionFunction::Always:
            mode = GL_ALWAYS;
            break;
        case ComparisionFunction::Equal:
            mode = GL_EQUAL;
            break;
        case ComparisionFunction::Greater:
            mode = GL_GREATER;
            break;
        case ComparisionFunction::GreaterOrEqual:
            mode = GL_GEQUAL;
            break;
        case ComparisionFunction::Less:
            mode = GL_LESS;
            break;
        case ComparisionFunction::LessOrEqual:
            mode = GL_LEQUAL;
            break;
        case ComparisionFunction::Never:
            mode = GL_NEVER;
            break;
        case ComparisionFunction::NotEqual:
            mode = GL_NOTEQUAL;
            break;
        default:
            Assertion(false, "Unhandled comparision function value!");
            mode = GL_ALWAYS;
            break;
    }
    return mode;
}
