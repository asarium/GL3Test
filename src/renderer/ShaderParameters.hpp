#pragma once

#include "Texture.hpp"
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

    virtual void setTexture(TextureHandle* handle) = 0;

    virtual void setUniformBuffer(BufferObject* object, size_t offset, size_t range) = 0;
};

enum class DescriptorSetType {
    ViewSet,
    ModelSet,
    HdrSet,
    LightingSet
};

enum class DescriptorSetPart {
    ViewSet_Uniforms,

    ModelSet_Uniforms,
    ModelSet_DiffuseTexture,

    HdrSet_BloomedTexture,

    // Lighting pass parts
    LightingSet_LightUniforms,
    LightingSet_PositionTexture,
    LightingSet_NormalTexture,
    LightingSet_AlbedoTexture,
    LightingSet_DirectionalShadowMap,
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
    glm::mat4 view_projection_matrix;
};

struct ModelUniformData {
    glm::mat4 model_matrix;
    glm::mat4 normal_model_matrix;
};

struct HdrUniformData {
    float exposure;
    uint32_t bloom_horizontal;
};
