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
    LightingSet,
    LightSet,
    NanoVGGlobalSet,
    NanoVGLocalSet
};

enum class DescriptorSetPart {
    ViewSet_Uniforms,

    ModelSet_Uniforms,
    ModelSet_DiffuseTexture,

    HdrSet_BloomedTexture,

    // Lighting pass parts
    LightingSet_PositionTexture,
    LightingSet_NormalTexture,
    LightingSet_AlbedoTexture,

    LightSet_Uniforms,
    LightSet_DirectionalShadowMap,

    // NanoVG parts
    NanoVGGlobalSet_Uniforms,
    
    NanoVGLocalSet_Uniforms,
    NanoVGLocalSet_Texture
};

class DescriptorSet {
 public:
    virtual ~DescriptorSet() {}

    virtual Descriptor* getDescriptor(DescriptorSetPart part) = 0;
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
