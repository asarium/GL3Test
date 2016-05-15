//
//

#include "LightingManager.hpp"

#include <renderer/RenderTargetManager.hpp>

#include <glm/gtc/matrix_transform.hpp>

namespace lighting {

Light::Light(Renderer* renderer, LightingManager* manager, LightType type)
    : _renderer(renderer), _manager(manager), _type(type) {
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
    params->light_has_shadow = 0;

}
LightType Light::getType() {
    return _type;
}

LightingManager::LightingManager(Renderer* renderer)
    : _renderer(renderer), _util(renderer), _alignedUniformData(renderer->getLimits().uniform_offset_alignment) {
    _alignedUniformData.allocate(128);

    _uniformBuffer = _renderer->createBuffer(BufferType::Uniform);
    _uniformBuffer->setData(nullptr, sizeof(LightParameters), BufferUsage::Streaming);

    _lightingDescriptorSet = _renderer->createDescriptorSet(DescriptorSetType::LightingSet);
    _lightingDataDescriptor = _lightingDescriptorSet->getDescriptor(DescriptorSetPart::LightingSet_LightUniforms);

    _lightingDataDescriptor->setUniformBuffer(_uniformBuffer.get(),
                                              0,
                                              sizeof(LightParameters));


    PipelineProperties pipelineProperties;
    pipelineProperties.shaderType = ShaderType::LightedMesh;

    pipelineProperties.depthMode = DepthMode::ReadWrite;
    pipelineProperties.depthFunction = DepthFunction::Less;

    pipelineProperties.blending = false;
    pipelineProperties.blendFunction = BlendFunction::None;

    _geometryPipelinestate = _renderer->createPipelineState(pipelineProperties);

    pipelineProperties.shaderType = ShaderType::LightingPass;

    pipelineProperties.depthMode = DepthMode::None;
    pipelineProperties.depthFunction = DepthFunction::Equal;

    pipelineProperties.blending = true;
    pipelineProperties.blendFunction = BlendFunction::Additive;

    _lightingPassPipelineState = _renderer->createPipelineState(pipelineProperties);
}
Light* LightingManager::addLight(LightType type, bool shadowing) {
    _lights.emplace_back(new Light(_renderer, this, type));
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

    auto currentRenderTarget = _renderer->getRenderTargetManager()->getCurrentRenderTarget();
    auto width = currentRenderTarget->getWidth();
    auto height = currentRenderTarget->getHeight();

    auto uv_scale = glm::vec2((float) width, (float) height)
        / glm::vec2(_lightingRenderTarget->getWidth(), _lightingRenderTarget->getHeight());
    auto inverse_window_size = 1.f / glm::vec2(width, height);

    auto frag_coord_scale = inverse_window_size * uv_scale;

    for (auto& light : _lights) {
        LightParameters lightParams;
        lightParams.frag_coord_scale = frag_coord_scale;
        light->setParameters(&lightParams);
        _uniformBuffer->updateData(&lightParams, 0, sizeof(lightParams), UpdateFlags::DiscardOldData);

        if (light->getType() == LightType::Point) {
            _util.drawSphere();
        } else {
            _util.drawFullscreenTri();
        }
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
    props.color_buffers = {
        ColorBufferFormat::RGB16F,
        ColorBufferFormat::RGB16F,
        ColorBufferFormat::RGBA
    };

    props.depth.enable = true;
    props.depth.make_texture_handle = false;

    _lightingRenderTarget = _renderer->getRenderTargetManager()->createRenderTarget(props);

    auto colorBuffers = _lightingRenderTarget->getColorTextures();
    Assertion(colorBuffers.size() == 3, "Number of color buffers does not match!");

    _lightingDescriptorSet->getDescriptor(DescriptorSetPart::LightingSet_PositionTexture)->setTexture(colorBuffers[0]);
    _lightingDescriptorSet->getDescriptor(DescriptorSetPart::LightingSet_NormalTexture)->setTexture(colorBuffers[1]);
    _lightingDescriptorSet->getDescriptor(DescriptorSetPart::LightingSet_AlbedoTexture)->setTexture(colorBuffers[2]);
}

}
