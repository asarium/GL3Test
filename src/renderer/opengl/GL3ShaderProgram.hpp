#pragma once

#include "renderer/ShaderProgram.hpp"
#include "GL3ShaderParameters.hpp"

#include <glad/glad.h>

#include <vector>
#include <util/FileLoader.hpp>
#include <renderer/ShaderParameters.hpp>

struct ParameterLocation {
    ShaderParameterType param;
    GLint location;
};

class GL3ShaderProgram : public ShaderProgram {
    GLuint _handle;
    std::vector<ParameterLocation> _uniformLocations;

    GLint getUniformLocation(ShaderParameterType param);
public:
    GL3ShaderProgram(FileLoader* loader, ShaderType type);
    virtual ~GL3ShaderProgram();

    void bind();

    void bindAndSetParameters(const GL3ShaderParameters* parameters);
};

