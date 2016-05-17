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
}

namespace lighting {
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
