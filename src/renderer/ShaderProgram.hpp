#pragma once

#include <glm/glm.hpp>

enum class ShaderType {
    Model
};

enum class ShaderParameter {
    ModelMatrix,
    ViewMatrix,
    ProjectionMatrix
};

class ShaderProgram {
public:
    virtual ~ShaderProgram() {};

    virtual void setParameterMat4(ShaderParameter param, const glm::mat4& value) = 0;
};


