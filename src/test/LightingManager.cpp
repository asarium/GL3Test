//
//

#include "LightingManager.hpp"

#include <renderer/RenderTargetManager.hpp>

#include <glm/gtc/matrix_transform.hpp>

namespace {
const uint32_t MAX_LIGHTS = 128;
std::unique_ptr<Texture> createTexture(Renderer* renderer, uint32_t width, uint32_t height, gli::format format) {
    auto texture = renderer->createTexture();

    AllocationProperties props;
    props.target = gli::TARGET_2D;
    props.size = gli::extent3d(width, height, 0);
    props.format = format;
    texture->allocate(props);

    return texture;
}
std::unique_ptr<Texture> createShadowMap(Renderer* renderer, uint32_t width, uint32_t height) {
    auto texture = renderer->createTexture();

    AllocationProperties props;
    props.target = gli::TARGET_2D;
    props.size = gli::extent3d(width, height, 0);
    props.format = gli::FORMAT_D24_UNORM_PACK32;

    props.compare_mode = TextureCompareMode::CompareRefToTexture;
    props.compare_func = ComparisionFunction::Less;

    props.wrap_behavior = WrapBehavior::ClampToBorder;
    props.border_color = glm::vec4(1.f, 1.f, 1.f, 1.f);
    texture->allocate(props);

    return texture;
}
}

namespace lighting {

Light::Light(Renderer* renderer, LightingManager* manager, LightType type, bool shadowing)
    : _renderer(renderer), _manager(manager), _type(type), _shadowing(shadowing) {
    _lightDescriptorSet = _renderer->createDescriptorSet(DescriptorSetType::LightSet);
    _uniformDescriptor = _lightDescriptorSet->getDescriptor(DescriptorSetPart::LightSet_Uniforms);
    _lightDescriptorSet->getDescriptor(DescriptorSetPart::LightSet_DirectionalShadowMap)->setTexture(nullptr);

    if (_shadowing) {
        PipelineProperties pipelineProperties;
        pipelineProperties.shaderType = ShaderType::ShadowMesh;

        pipelineProperties.depthFunction = ComparisionFunction::Less;
        pipelineProperties.depthMode = DepthMode::ReadWrite;

        pipelineProperties.blendFunction = BlendFunction::None;
        pipelineProperties.blending = false;

        _shadowPassPipelinestate = _renderer->createPipelineState(pipelineProperties);

        RenderTargetProperties props;
        props.width = 1024;
        props.height = 1024;
        props.depth_texture = createShadowMap(_renderer, 1024, 1024);

        _shadowMapTarget = _renderer->getRenderTargetManager()->createRenderTarget(std::move(props));
        _lightDescriptorSet->getDescriptor(DescriptorSetPart::LightSet_DirectionalShadowMap)->setTexture(
            _shadowMapTarget->getDepthTexture());
    }
}
void Light::setPosition(const glm::vec3& pos) {
    _position = pos;
}
void Light::setDirection(const glm::vec3& dir) {
    _direction = glm::normalize(dir);
}
void Light::setColor(const glm::vec3& color) {
    _color = color;
}
void Light::setParameters(LightParameters* params) {
    switch (_type) {
        case LightType::Directional:
            params->light_type = 0;
            params->light_vector = _direction;
            break;
        case LightType::Point: {
            params->light_type = 1;
            params->light_vector = _position;

            const float cutoff = 0.01f; // Cutoff value after which this light does not affect anything anymore
            // This depends on the formula in the shader!
            auto scale = sqrtf((glm::length(_color) / cutoff) - 1.f) * 1.2f;
            glm::mat4 lightSphere;
            lightSphere = glm::translate(lightSphere, _position);
            lightSphere = glm::scale(lightSphere, glm::vec3(scale));
            params->model_matrix = lightSphere;
            break;
        }
        case LightType::Ambient:
            params->light_type = 2;
            break;
        default:
            params->light_type = 0;
            break;
    }

    params->light_color = _color;
    params->light_has_shadow = _shadowing ? 1 : 0;
    if (_shadowing)
    {
        float near_plane = 1.0f, far_plane = 70.f;
        _matricies.projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        _matricies.view = glm::lookAt(_direction * 10.f,
                                      glm::vec3(0.0f, 0.0f, 0.0f),
                                      glm::vec3(0.0f, 1.0f, 0.0f));
        params->light_view_proj_matrix = _matricies.projection * _matricies.view;
    }

}
LightType Light::getType() {
    return _type;
}
ShadowMatrices Light::beginShadowPass() {
    Assertion(_shadowing, "Shadowing is not enabled for this light!");

    _renderer->getRenderTargetManager()->pushRenderTargetBinding();
    _renderer->getRenderTargetManager()->useRenderTarget(_shadowMapTarget.get());
    _renderer->clear(glm::vec4(0.f));

    _shadowPassPipelinestate->bind();

    return _matricies;
}
void Light::endShadowPass() {
    Assertion(_shadowing, "Shadowing is not enabled for this light!");
    _renderer->getRenderTargetManager()->popRenderTargetBinding();
}
void Light::updateDescriptor(BufferObject* uniforms, size_t offset, size_t size) {
    _uniformDescriptor->setUniformBuffer(uniforms,
                                         offset,
                                         size);
}
void Light::bindDescriptorSet() {
    _lightDescriptorSet->bind();
}
void Light::unbindDescriptorSet() {
    _lightDescriptorSet->unbind();
}


LightingManager::LightingManager(Renderer* renderer)
    : _renderer(renderer), _util(renderer), _alignedUniformData(renderer->getLimits().uniform_offset_alignment) {
    _alignedUniformData.allocate(MAX_LIGHTS);

    _uniformBuffer = _renderer->createBuffer(BufferType::Uniform);
    _uniformBuffer->setData(nullptr, _alignedUniformData.getSize(), BufferUsage::Dynamic);

    _lightingDescriptorSet = _renderer->createDescriptorSet(DescriptorSetType::LightingSet);

    auto current = _renderer->getRenderTargetManager()->getCurrentRenderTarget();

    ensureRenderTargetSize(current->getWidth(), current->getHeight());

    {
        PipelineProperties pipelineProperties;
        pipelineProperties.shaderType = ShaderType::LightedMesh;

        pipelineProperties.depthMode = DepthMode::ReadWrite;
        pipelineProperties.depthFunction = ComparisionFunction::Less;

        pipelineProperties.blending = false;
        pipelineProperties.blendFunction = BlendFunction::None;

        _geometryPipelinestate = _renderer->createPipelineState(pipelineProperties);
    }
    {
        PipelineProperties pipelineProperties;
        pipelineProperties.shaderType = ShaderType::LightingPass;

        pipelineProperties.depthMode = DepthMode::None;
        pipelineProperties.depthFunction = ComparisionFunction::Equal;

        pipelineProperties.blending = true;
        pipelineProperties.blendFunction = BlendFunction::Additive;

        _lightingPassPipelineState = _renderer->createPipelineState(pipelineProperties);
    }
}
Light* LightingManager::addLight(LightType type, bool shadowing) {
    if (shadowing && type != LightType::Directional) {
        return nullptr;
    }

    _lights.emplace_back(new Light(_renderer, this, type, shadowing));
    return _lights.back().get();
}
void LightingManager::removeLight(Light* light) {
    for (auto iter = _lights.begin(); iter != _lights.end(); ++iter) {
        if (iter->get() == light) {
            _lights.erase(iter);
            break;
        }
    }
}
void LightingManager::clearLights() {
    _lights.clear();
}
void LightingManager::beginLightPass() {
    auto current = _renderer->getRenderTargetManager()->getCurrentRenderTarget();

    ensureRenderTargetSize(current->getWidth(), current->getHeight());

    _renderer->getRenderTargetManager()->pushRenderTargetBinding();
    _renderer->getRenderTargetManager()->useRenderTarget(_lightingRenderTarget.get());
    _renderer->clear(glm::vec4(0.f, 0.f, 0.f, 1.f));

    _geometryPipelinestate->bind();
}
void LightingManager::endLightPass() {
    _renderer->getRenderTargetManager()->popRenderTargetBinding();
    _renderer->clear(glm::vec4(0.f, 0.f, 0.f, 1.f));

    _lightingPassPipelineState->bind();

    _lightingDescriptorSet->bind();

    for (auto& light : _lights) {
        light->bindDescriptorSet();
        if (light->getType() == LightType::Point) {
            _util.drawSphere();
        } else {
            _util.drawFullscreenTri();
        }
        light->unbindDescriptorSet();
    }

    _lightingDescriptorSet->unbind();
}
void LightingManager::ensureRenderTargetSize(size_t width, size_t height) {
    if (_lightingRenderTarget
        && (_lightingRenderTarget->getWidth() >= width || _lightingRenderTarget->getHeight() >= height)) {
        // Already big enough
        return;
    }

    RenderTargetProperties props;
    props.width = width;
    props.height = height;
    props.color_buffers.push_back(std::move(createTexture(_renderer, width, height, gli::FORMAT_RGB16_SFLOAT_PACK16)));
    props.color_buffers.push_back(std::move(createTexture(_renderer, width, height, gli::FORMAT_RGB16_SFLOAT_PACK16)));
    props.color_buffers.push_back(std::move(createTexture(_renderer, width, height, gli::FORMAT_RGBA8_UNORM_PACK8)));

    props.depth_texture = createTexture(_renderer, width, height, gli::FORMAT_D24_UNORM_PACK32);

    _lightingRenderTarget = _renderer->getRenderTargetManager()->createRenderTarget(std::move(props));

    auto colorBuffers = _lightingRenderTarget->getColorTextures();
    Assertion(colorBuffers.size() == 3, "Number of color buffers does not match!");

    _lightingDescriptorSet->getDescriptor(DescriptorSetPart::LightingSet_PositionTexture)->setTexture(colorBuffers[0]);
    _lightingDescriptorSet->getDescriptor(DescriptorSetPart::LightingSet_NormalTexture)->setTexture(colorBuffers[1]);
    _lightingDescriptorSet->getDescriptor(DescriptorSetPart::LightingSet_AlbedoTexture)->setTexture(colorBuffers[2]);
}
void LightingManager::updateLightData() {
    auto currentRenderTarget = _renderer->getRenderTargetManager()->getCurrentRenderTarget();
    auto width = currentRenderTarget->getWidth();
    auto height = currentRenderTarget->getHeight();

    auto uv_scale = glm::vec2((float) width, (float) height)
        / glm::vec2(_lightingRenderTarget->getWidth(), _lightingRenderTarget->getHeight());
    auto inverse_window_size = 1.f / glm::vec2(width, height);

    auto frag_coord_scale = inverse_window_size * uv_scale;

    size_t i = 0;
    for (auto& light : _lights) {
        LightParameters* params = _alignedUniformData.getElement(i);
        params->frag_coord_scale = frag_coord_scale;
        light->setParameters(params);
        light->updateDescriptor(_uniformBuffer.get(), _alignedUniformData.getOffset(i), sizeof(LightParameters));
        ++i;
    }

    _uniformBuffer->updateData(_alignedUniformData.getData(),
                               0,
                               _alignedUniformData.getSize(),
                               UpdateFlags::DiscardOldData);
}

}
