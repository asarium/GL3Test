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

    virtual void setInteger(ShaderParameterType param, int value) override;

    virtual void setFloat(ShaderParameterType param, float value) override;

    virtual void setVec2(ShaderParameterType param, const glm::vec2& value) override;

    virtual void setVec3(ShaderParameterType param, const glm::vec3& value) override;

    virtual void setMat4(ShaderParameterType param, const glm::mat4 &value) override;

    virtual void setTexture(ShaderParameterType param, Texture2D* value) override;


    void set2dTextureHandle(GL3ShaderParameterType param, GLuint handle);

    void setInteger(GL3ShaderParameterType param, GLint value);

    void setFloat(GL3ShaderParameterType param, GLfloat value);

    void setVec2(GL3ShaderParameterType param, const glm::vec2& value);

    void setVec3(GL3ShaderParameterType param, const glm::vec3& value);

    void setMat4(GL3ShaderParameterType param, const glm::mat4 &value);

    void setTexture(GL3ShaderParameterType param, Texture2D* value);

    const std::vector<ParameterValue>& getValues() const;
};


