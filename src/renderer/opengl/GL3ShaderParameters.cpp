//
//

#include "GL3ShaderParameters.hpp"
#include "GL3ShaderProgram.hpp"

#include "EnumTranslation.hpp"

GL3ShaderParameters::GL3ShaderParameters() {
}

GL3ShaderParameters::~GL3ShaderParameters() {

}

ParameterValue &GL3ShaderParameters::getValue(GL3ShaderParameterType type) {
    for (auto &entry : _values) {
        if (entry.param_type == type) {
            return entry;
        }
    }
    _values.emplace_back();
    return _values.back();
}

void GL3ShaderParameters::setVec2(ShaderParameterType param, const glm::vec2 &value) {
    auto type = convertParameterType(param);
    auto &val = getValue(type);
    val.param_type = type;
    val.data_type = ParameterDataType::Vec2;
    val.value.vec2 = value;
}

void GL3ShaderParameters::setMat4(ShaderParameterType param, const glm::mat4 &value) {
    auto type = convertParameterType(param);
    auto &val = getValue(type);
    val.param_type = type;
    val.data_type = ParameterDataType::Mat4;
    val.value.mat4 = value;
}

void GL3ShaderParameters::setTexture(ShaderParameterType param, Texture2D *value) {
    auto type = convertParameterType(param);
    auto &val = getValue(type);
    val.param_type = type;
    val.data_type = ParameterDataType::Tex2D;
    val.value.tex2d = static_cast<GL3Texture2D *>(value);
}

void GL3ShaderParameters::set2dTextureHandle(GL3ShaderParameterType param, GLuint handle) {
    auto &val = getValue(param);
    val.param_type = param;
    val.data_type = ParameterDataType::Tex2DHandle;
    val.value.tex2dhandle = handle;
}

void GL3ShaderParameters::setInteger(GL3ShaderParameterType param, GLint value) {
    auto &val = getValue(param);
    val.param_type = param;
    val.data_type = ParameterDataType::Integer;
    val.value.integer = value;
}

void GL3ShaderParameters::setFloat(GL3ShaderParameterType param, GLfloat value) {
    auto &val = getValue(param);
    val.param_type = param;
    val.data_type = ParameterDataType::Float;
    val.value.floatVal = value;
}

void GL3ShaderParameters::setVec3(GL3ShaderParameterType param, const glm::vec3 &value) {
    auto &val = getValue(param);
    val.param_type = param;
    val.data_type = ParameterDataType::Vec3;
    val.value.vec3 = value;
}

const std::vector<ParameterValue> &GL3ShaderParameters::getValues() const {
    return _values;
}






