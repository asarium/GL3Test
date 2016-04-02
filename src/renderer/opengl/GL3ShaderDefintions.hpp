#pragma once

#include <renderer/DrawCallManager.hpp>
#include <renderer/VertexLayout.hpp>

#include <glad/glad.h>

#include "Enums.hpp"

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

struct GL3ShaderDefinition {
    std::vector<ShaderFilename> filenames;

    std::vector<UniformMapping> uniforms;

    std::vector<AttributeBinding> attribute_bindings;
};

GL3ShaderDefinition getShaderDefinition(GL3ShaderType type);
