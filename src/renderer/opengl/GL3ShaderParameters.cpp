//
//

#include "GL3ShaderParameters.hpp"

GL3ShaderParameters::GL3ShaderParameters() {

}

GL3ShaderParameters::~GL3ShaderParameters() {

}

void GL3ShaderParameters::setParameterMat4(ShaderParameterType param, const glm::mat4 &value) {
    ParameterValue val;
    val.param_type = param;
    val.data_type = ParameterDataType::Mat4;
    val.value.mat4 = value;

    _values.push_back(val);
}

const std::vector<ParameterValue> &GL3ShaderParameters::getValues() const {
    return _values;
}



