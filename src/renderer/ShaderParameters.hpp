#pragma once

#include <glm/glm.hpp>

enum class ShaderParameterType {
    ModelMatrix,
    ViewMatrix,
    ProjectionMatrix
};

class ShaderParameters {
public:
    virtual ~ShaderParameters() { }

    virtual void setParameterMat4(ShaderParameterType param, const glm::mat4& value) = 0;
};
