//
//

#include "GL3ShaderParameters.hpp"

GL3ShaderParameters::GL3ShaderParameters() {

}

GL3ShaderParameters::~GL3ShaderParameters() {

}

void GL3ShaderParameters::setMat4(ShaderParameterType param, const glm::mat4 &value) {
    ParameterValue val;
    val.param_type = param;
    val.data_type = ParameterDataType::Mat4;
    val.value.mat4 = value;

    _values.push_back(val);
}

void GL3ShaderParameters::setTexture(ShaderParameterType param, Texture2D *value) {
    ParameterValue val;
    val.param_type = param;
    val.data_type = ParameterDataType::Tex2D;
    val.value.tex2d = static_cast<GL3Texture2D*>(value);

    _values.push_back(val);
}

const std::vector<ParameterValue> &GL3ShaderParameters::getValues() const {
    return _values;
}





