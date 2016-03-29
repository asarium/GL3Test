#pragma once
//
//

#include <vector>
#include <cstring>
#include "renderer/ShaderParameters.hpp"
#include "GL3Texture2D.hpp"

enum class ParameterDataType {
    Vec2,
    Mat4,
    Tex2D
};

struct ParameterValue {
    ShaderParameterType param_type;
    ParameterDataType data_type;

    struct {
        glm::vec2 vec2;
        glm::mat4 mat4;
        GL3Texture2D* tex2d;
    } value;
};

class GL3ShaderParameters : public ShaderParameters {
    std::vector<ParameterValue> _values;

    ParameterValue& getValue(ShaderParameterType type);
public:
    GL3ShaderParameters();

    virtual ~GL3ShaderParameters();

    virtual void setVec2(ShaderParameterType param, const glm::vec2& value) override;

    virtual void setMat4(ShaderParameterType param, const glm::mat4 &value) override;

    virtual void setTexture(ShaderParameterType param, Texture2D* value) override;

    const std::vector<ParameterValue>& getValues() const;
};


