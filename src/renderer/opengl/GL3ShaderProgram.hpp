#pragma once

#include "renderer/ShaderProgram.hpp"
#include "GL3ShaderParameters.hpp"

#include <glad/glad.h>

#include <vector>
#include <util/FileLoader.hpp>
#include <renderer/ShaderParameters.hpp>

#include <unordered_map>

enum class GL3ShaderTypes {
    DeferredLightningPass
};

const size_t NUM_SHADER_PARAMETER = 5; // Must match the actual number of parameters!

class GL3ShaderProgram : public ShaderProgram {
    GLuint _handle;
    GLint _uniformLocations[NUM_SHADER_PARAMETER];

    GLint getUniformLocation(ShaderParameterType param);
public:
    GL3ShaderProgram(FileLoader* loader, ShaderType type);
    virtual ~GL3ShaderProgram();

    void bind();

    void bindAndSetParameters(const GL3ShaderParameters* parameters);

    static size_t getParameterIndex(ShaderParameterType type);
};

