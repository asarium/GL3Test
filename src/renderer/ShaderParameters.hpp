#pragma once

#include <glm/glm.hpp>
#include "Texture2D.hpp"

enum class ShaderParameterType {
    ModelMatrix,
    ViewMatrix,
    ProjectionMatrix,
    ColorTexture,
    WindowSize
};

class ShaderParameters {
public:
    virtual ~ShaderParameters() { }

    virtual void setVec2(ShaderParameterType param, const glm::vec2& value) = 0;

    virtual void setMat4(ShaderParameterType param, const glm::mat4& value) = 0;

    virtual void setTexture(ShaderParameterType param, Texture2D* value) = 0;
};
