#pragma once

#include <renderer/DrawCallManager.hpp>
#include <renderer/VertexLayout.hpp>

#include <glad/glad.h>

#include "Enums.hpp"
#include "GL3ShaderParameters.hpp"

#include <vector>

struct ShaderFilename {
    GLenum type;
    const char *filename;
};

struct AttributeBinding {
    AttributeType type;
    const char* name;
    GLint binding_location;
};

struct UniformMapping {
    GL3ShaderParameterType parameter;
    const char* name;
};

struct UniformRequirements {
    GL3ShaderParameterType checked_type;
    std::vector<ParameterDataType> acceptable_datatypes;
};

struct GL3ShaderDefinition {
    std::vector<ShaderFilename> filenames;

    std::vector<UniformMapping> uniforms;

    std::vector<AttributeBinding> attribute_bindings;

    std::vector<UniformRequirements> uniform_requirements;
};

GL3ShaderDefinition getShaderDefinition(GL3ShaderType type);

std::vector<GL3ShaderType> getDefinedShaderTypes();
