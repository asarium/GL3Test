#pragma once

#include "GL3ShaderParameters.hpp"
#include "GL3ShaderDefintions.hpp"

#include <glad/glad.h>

#include <util/FileLoader.hpp>
#include <renderer/ShaderParameters.hpp>
#include <renderer/DrawCallManager.hpp>

#include <vector>
#include <unordered_map>

class GL3ShaderProgram {
    GLuint _handle;
    GLint _uniformLocations[num_enum_values<GL3ShaderParameterType>()];

    inline GLint getUniformLocation(GL3ShaderParameterType param) {
        return _uniformLocations[static_cast<size_t>(param)];
    }

    std::vector<UniformRequirements> _requirements;

 public:
    GL3ShaderProgram(FileLoader* loader, const GL3ShaderDefinition& definition);
    ~GL3ShaderProgram();

    void bind();

    void bindAndSetParameters(const GL3ShaderParameters* parameters);
};

