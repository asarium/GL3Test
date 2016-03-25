#pragma once

#include "renderer/ShaderProgram.hpp"

#include <glad/glad.h>

#include <vector>
#include <util/FileLoader.hpp>

struct ParameterLocation {
    ShaderParameter param;
    GLint location;
};

class GL3ShaderProgram : public ShaderProgram {
    GLuint _handle;
    std::vector<ParameterLocation> _uniformLocations;

    GLint getUniformLocation(ShaderParameter param);
public:
    GL3ShaderProgram(FileLoader* loader, ShaderType type);
    virtual ~GL3ShaderProgram();

    void bind();

    virtual void setParameterMat4(ShaderParameter param, const glm::mat4 &value) override;
};

