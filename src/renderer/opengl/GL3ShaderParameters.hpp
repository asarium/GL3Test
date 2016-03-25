#pragma once
//
//

#include <vector>
#include <cstring>
#include "renderer/ShaderParameters.hpp"

enum class ParameterDataType {
    Mat4
};

struct ParameterValue {
    ShaderParameterType param_type;
    ParameterDataType data_type;

    struct {
        glm::mat4 mat4;
    } value;
};

class GL3ShaderParameters : public ShaderParameters {
    std::vector<ParameterValue> _values;
public:
    GL3ShaderParameters();

    virtual ~GL3ShaderParameters();

    virtual void setParameterMat4(ShaderParameterType param, const glm::mat4 &value) override;

    const std::vector<ParameterValue> &getValues() const;
};


