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

 public:
    GL3ShaderProgram(GLuint handle, const GL3ShaderDefinition& definition);
    ~GL3ShaderProgram();

    void bind();

    void bindAndSetParameters(const GL3ShaderParameters* parameters);

    inline GLint getUniformLocation(GL3ShaderParameterType param) {
        return _uniformLocations[static_cast<size_t>(param)];
    }

    inline GLuint getHandle() {
        return _handle;
    }
};

