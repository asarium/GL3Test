#pragma once

#include <renderer/DrawCallManager.hpp>
#include <renderer/VertexLayout.hpp>

#include <glad/glad.h>

#include "Enums.hpp"
#include "GL3ShaderParameters.hpp"

#include <vector>
#include <renderer/PipelineState.hpp>

struct ShaderFilename {
    GLenum type;
    const char *filename;
};

struct AttributeBinding {
    AttributeType type;
    const char* name;
    GLuint binding_location;
};

struct DescriptorBinding
{
    GL3DescriptorSetPart part;
    const char* name;
    GLuint location;
};

struct GL3ShaderDefinition {
    std::vector<ShaderFilename> filenames;

    std::vector<AttributeBinding> attribute_bindings;

    std::vector<DescriptorBinding> buffer_bindings;

    std::vector<DescriptorBinding> texture_bindings;
};

GL3ShaderDefinition getShaderDefinition(ShaderType type);

std::vector<ShaderType> getDefinedShaderTypes();
