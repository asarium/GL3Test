
#include "Light.hpp"
#include <util/Assertion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace {
    std::unique_ptr<Texture> createShadowMap(Renderer* renderer, uint32_t width, uint32_t height) {
        auto texture = renderer->createTexture();

        AllocationProperties props;
        props.target = gli::TARGET_2D;
        props.size = gli::extent3d(width, height, 0);
        props.format = gli::FORMAT_D24_UNORM_PACK32;

        props.compare_mode = TextureCompareMode::CompareRefToTexture;
        props.compare_func = ComparisionFunction::Less;

        props.filterProperties.wrap_behavior_s = WrapBehavior::ClampToBorder;
        props.filterProperties.wrap_behavior_t = WrapBehavior::ClampToBorder;
        props.filterProperties.wrap_behavior_r = WrapBehavior::ClampToBorder;
        props.filterProperties.border_color = glm::vec4(1.f, 1.f, 1.f, 1.f);
        texture->allocate(props);

        return texture;
    }
}

namespace lighting
{
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
            pipelineProperties.enableBlending = false;

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
        _renderer->clear(glm::vec4(0.f), ClearTarget::Depth);

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

}
