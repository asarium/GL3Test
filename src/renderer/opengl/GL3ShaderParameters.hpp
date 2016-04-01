#pragma once
//
//

#include <vector>
#include <cstring>

#include "renderer/ShaderParameters.hpp"

#include "Enums.hpp"
#include "GL3Texture2D.hpp"

enum class ParameterDataType {
    Float,
    Integer,
    Vec2,
    Vec3,
    Mat4,
    Tex2D,
    Tex2DHandle,
};

struct ParameterValue {
    GL3ShaderParameterType param_type;
    ParameterDataType data_type;

    struct {
        glm::vec2 vec2;
        glm::vec3 vec3;
        glm::mat4 mat4;
        GL3Texture2D* tex2d;
        GLuint tex2dhandle;
        GLint integer;
        GLfloat floatVal;
    } value;
};

class GL3ShaderParameters : public ShaderParameters {
    std::vector<ParameterValue> _values;

    ParameterValue& getValue(GL3ShaderParameterType type);
public:
    GL3ShaderParameters();

    virtual ~GL3ShaderParameters();

    void set2dTextureHandle(GL3ShaderParameterType param, GLuint handle);

    void setInteger(GL3ShaderParameterType param, GLint value);

    void setFloat(GL3ShaderParameterType param, GLfloat value);

    virtual void setVec2(ShaderParameterType param, const glm::vec2& value) override;

    void setVec3(GL3ShaderParameterType param, const glm::vec3& value);

    virtual void setMat4(ShaderParameterType param, const glm::mat4 &value) override;

    virtual void setTexture(ShaderParameterType param, Texture2D* value) override;

    const std::vector<ParameterValue>& getValues() const;
};


