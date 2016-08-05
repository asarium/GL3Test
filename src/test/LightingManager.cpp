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
    _alignedUniformData.resize(MAX_LIGHTS);

    _uniformBuffer = _renderer->createBuffer(BufferType::Uniform);
    _uniformBuffer->setData(nullptr, _alignedUniformData.getSize(), BufferUsage::Dynamic);

    _lightingDescriptorSet = _renderer->createDescriptorSet(DescriptorSetType::LightingSet);

    auto current = _renderer->getRenderTargetManager()->getCurrentRenderTarget();

    ensureRenderTargetSize(current->getWidth(), current->getHeight());

    {
        PipelineProperties pipelineProperties;
        pipelineProperties.shaderType = ShaderType::LightingPass;

        pipelineProperties.depthMode = DepthMode::None;
        pipelineProperties.depthFunction = ComparisionFunction::Equal;

        pipelineProperties.enableBlending = true;
        pipelineProperties.blendFunction = BlendFunction::Additive;

        _util.setVertexProperties(pipelineProperties);

        _lightingPassPipelineState = _renderer->createPipelineState(pipelineProperties);

        _sphereMesh = _util.getSphereMesh();
        _fullscreenTriMesh = _util.getFullscreenTriMesh();
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
void LightingManager::beginLightPass(CommandBuffer* cmd) {
    _renderer->getDebugging()->pushGroup("Light pass");

    auto current = _renderer->getRenderTargetManager()->getCurrentRenderTarget();

    ensureRenderTargetSize(current->getWidth(), current->getHeight());

    _renderer->getRenderTargetManager()->pushRenderTargetBinding();
    _renderer->getRenderTargetManager()->useRenderTarget(_lightingRenderTarget.get());

    cmd->clear(glm::vec4(0.f, 0.f, 0.f, 1.f), ClearTarget::Color | ClearTarget::Depth);
}
void LightingManager::endLightPass(CommandBuffer* cmd) {
    _renderer->getRenderTargetManager()->popRenderTargetBinding();
    cmd->clear(glm::vec4(0.f, 0.f, 0.f, 1.f), ClearTarget::Color);

    cmd->bindPipeline(_lightingPassPipelineState.get());

    cmd->bindDescriptorSet(_lightingDescriptorSet.get());

    {
        DEBUG_SCOPE(lights, _renderer->getDebugging(), "Render lights");

        for (auto& light : _lights) {
            light->bindDescriptorSet(cmd);
            if (light->getType() == LightType::Point) {
                _sphereMesh->draw(cmd, 1);
            } else {
                _fullscreenTriMesh->draw(cmd, 1);
            }
        }
    }

    cmd->unbindDescriptorSet(_lightingDescriptorSet);

    _renderer->getDebugging()->popGroup();
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

PipelineProperties LightingManager::getGeometryProperties() const {
    PipelineProperties pipelineProperties;
    pipelineProperties.shaderType = ShaderType::LightedMesh;

    pipelineProperties.depthMode = DepthMode::ReadWrite;
    pipelineProperties.depthFunction = ComparisionFunction::Less;

    pipelineProperties.enableBlending = false;
    pipelineProperties.blendFunction = BlendFunction::None;

    return pipelineProperties;
}

}
