#pragma once

#include "Texture2D.hpp"
#include "RenderTarget.hpp"
#include "BufferObject.hpp"

#include <glm/glm.hpp>

enum class DescriptorType {
    UniformBuffer,
    Texture
};

class Descriptor {
 public:
    virtual ~Descriptor() {}

    virtual void setTexture(Texture2DHandle* handle) = 0;

    virtual void setUniformBuffer(BufferObject* object, size_t offset, size_t range) = 0;
};

enum class DescriptorSetType {
    ViewSet,
    ModelSet,
    HdrSet
};

enum class DescriptorSetPart {
    ViewSet_Uniforms,

    ModelSet_Uniforms,
    ModelSet_DiffuseTexture,

    HdrSet_Uniforms,
    HdrSet_BloomedTexture
};

class DescriptorSet {
 public:
    virtual ~DescriptorSet() {}

    virtual Descriptor* getDescriptor(DescriptorSetPart part) = 0;

    virtual void bind() = 0;

    virtual void unbind() = 0;
};

struct ViewUniformData {
    glm::mat4 projection_matrix;
    glm::mat4 view_matrix;
};

struct ModelUniformData {
    glm::mat4 model_matrix;
};

struct HdrUniformData {
    float exposure;
    uint32_t bloom_horizontal;
};

enum class ShaderParameterType {
    ModelMatrix,
    ViewMatrix,
    ProjectionMatrix,
    ColorTexture,
    WindowSize,
    HdrExposure,
    BloomHorizontal,
    BloomedTexture
};

class ShaderParameters {
public:
    virtual ~ShaderParameters() { }

    virtual void setBoolean(ShaderParameterType param, bool value) = 0;

    virtual void setInteger(ShaderParameterType param, int value) = 0;

    virtual void setFloat(ShaderParameterType param, float value) = 0;

    virtual void setVec2(ShaderParameterType param, const glm::vec2& value) = 0;

    virtual void setVec3(ShaderParameterType param, const glm::vec3& value) = 0;

    virtual void setMat4(ShaderParameterType param, const glm::mat4 &value) = 0;

    virtual void setTexture(ShaderParameterType param, Texture2DHandle* value) = 0;
};
