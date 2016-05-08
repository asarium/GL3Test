#pragma once
//
//

#include "renderer/ShaderParameters.hpp"

#include "Enums.hpp"
#include "GL3Texture2D.hpp"
#include "GL3BufferObject.hpp"

#include <glad/glad.h>

#include <vector>
#include <cstring>
#include <unordered_map>

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

class GL3ShaderParameters final: public ShaderParameters {
    std::vector<ParameterValue> _values;

    ParameterValue& getValue(GL3ShaderParameterType type);
 public:
    GL3ShaderParameters();

    virtual ~GL3ShaderParameters();

    virtual void setBoolean(ShaderParameterType param, bool value) override;

    virtual void setInteger(ShaderParameterType param, int value) override;

    virtual void setFloat(ShaderParameterType param, float value) override;

    virtual void setVec2(ShaderParameterType param, const glm::vec2& value) override;

    virtual void setVec3(ShaderParameterType param, const glm::vec3& value) override;

    virtual void setMat4(ShaderParameterType param, const glm::mat4& value) override;

    virtual void setTexture(ShaderParameterType param, Texture2DHandle* value) override;


    void set2dTextureHandle(GL3ShaderParameterType param, GLuint handle);

    void setInteger(GL3ShaderParameterType param, GLint value);

    void setFloat(GL3ShaderParameterType param, GLfloat value);

    void setVec2(GL3ShaderParameterType param, const glm::vec2& value);

    void setVec3(GL3ShaderParameterType param, const glm::vec3& value);

    void setMat4(GL3ShaderParameterType param, const glm::mat4& value);

    void setTexture(GL3ShaderParameterType param, GL3Texture2D* value);

    const std::vector<ParameterValue>& getValues() const;
};


class GL3Descriptor final: public Descriptor {
public:
    struct Data
    {
        DescriptorType type;
        GL3DescriptorSetPart part;
        struct
        {
            GL3TextureHandle texture;
            struct
            {
                GL3BufferObject* buffer;
                GLintptr offset;
                GLsizei size;
            } buffer;
        } descriptor_data;
    };

private:
    Data _data;
    bool _active;
public:
    explicit GL3Descriptor(GL3DescriptorSetPart part);
    virtual ~GL3Descriptor() {}

    void setTexture(Texture2DHandle* handle) override;

    void setGLTexture(const GL3TextureHandle& handle);

    void setUniformBuffer(BufferObject* object, size_t offset, size_t range) override;

    void bind();

    void unbind();

    const Data& getData() const
    {
        return _data;
    }
};

class GL3DescriptorSet final: public DescriptorSet {
    Gl3DescriptorSetType _type;
    bool _active;

    std::unordered_map<GL3DescriptorSetPart, std::unique_ptr<GL3Descriptor>> _descriptors;
public:
    explicit GL3DescriptorSet(Gl3DescriptorSetType type);
    virtual ~GL3DescriptorSet() {}

    GL3Descriptor* getDescriptor(GL3DescriptorSetPart part);

    Descriptor* getDescriptor(DescriptorSetPart part) override;

    void bind() override;

    void unbind() override;
};

