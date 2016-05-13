//
//

#include "GL3LightingManager.hpp"
#include "GL3Renderer.hpp"
#include "GL3State.hpp"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <util/Assertion.hpp>

namespace
{
    struct GlobalParameters
    {
        glm::vec2 window_size;
        glm::vec2 uv_scale;
    };
}

GL3LightingManager::GL3LightingManager(GL3Renderer* renderer) : GL3Object(renderer), _renderFrameBuffer(0),
                                                                _gBufferTextures{ 0 }, _depthRenderBuffer(0),
                                                                _shadowMapResolution(0),
                                                                _dirty(false) {
}

GL3LightingManager::~GL3LightingManager() {
    freeResources();
}

bool GL3LightingManager::initialize() {
    PipelineProperties pipelineProperties;
    pipelineProperties.shaderType = ShaderType::LightedMesh;

    pipelineProperties.depthMode = DepthMode::ReadWrite;
    pipelineProperties.depthFunction = DepthFunction::Less;

    pipelineProperties.blending = false;
    pipelineProperties.blendFunction = BlendFunction::None;

    _geometryPipelineState = _renderer->createPipelineState(pipelineProperties);

    _globalUniformBuffer = _renderer->createBuffer(BufferType::Uniform);
    _globalUniformBuffer->setData(nullptr, sizeof(GlobalParameters), BufferUsage::Streaming);

    _lightUniformBuffer = _renderer->createBuffer(BufferType::Uniform);
    _lightUniformBuffer->setData(nullptr, sizeof(GL3Light::LightParameters), BufferUsage::Streaming);

    _lightingParameterSet.reset(new GL3DescriptorSet(Gl3DescriptorSetType::LightingSet));

    _shadowMapDescriptor = _lightingParameterSet->getDescriptor(GL3DescriptorSetPart::LightingSet_DiretionalShadowMap);
    _shadowMapDescriptor->setGLTexture(GL3TextureHandle(GL_TEXTURE_2D, 0));

    _lightingParameterSet->getDescriptor(GL3DescriptorSetPart::LightingSet_GlobalUniforms)->
        setUniformBuffer(_globalUniformBuffer.get(), 0, sizeof(GlobalParameters));
    _lightingParameterSet->getDescriptor(GL3DescriptorSetPart::LightingSet_LightUniforms)->
        setUniformBuffer(_lightUniformBuffer.get(), 0, sizeof(GL3Light::LightParameters));

    return true;
}

void GL3LightingManager::markDirty()
{
    _dirty = true;
}

Light* GL3LightingManager::addLight(LightType type, bool shadows) {
    if (shadows && type != LightType::Directional) {
        return nullptr; // Shadows are only supported for directional lights currently
    }

    _lights.emplace_back(new GL3Light(this->_renderer, this, type, shadows ? _shadowMapResolution : 0));

    markDirty();

    return _lights.back().get();
}

void GL3LightingManager::removeLight(Light* light) {
    for (auto iter = _lights.begin(); iter != _lights.end(); ++iter) {
        if (iter->get() == light) {
            _lights.erase(iter);
            markDirty();
            break;
        }
    }
}

void GL3LightingManager::clearLights() {
    _lights.clear();
}

void GL3LightingManager::beginLightPass() {
    auto currentRenderTarget = _renderer->getGLRenderTargetManager()->getCurrentRenderTarget();
    auto width = currentRenderTarget->getWidth();
    auto height = currentRenderTarget->getHeight();

    // Make sure our framebuffer is big enough for the current render target
    // This check might be a bit late but if someone uses a render target without lighting then changing the size when
    // the render target is created would waste memory
    resizeFramebuffer((uint32_t) width, (uint32_t) height);

    GLState->Framebuffer.pushBinding();
    GLState->Framebuffer.bind(_renderFrameBuffer);

    glClearColor(0.f, 0.f, 0.f, 1.f);
    GLState->setDepthMask(true);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height);
    _geometryPipelineState->bind();
}

void GL3LightingManager::endLightPass() {
    GLState->Framebuffer.popBinding();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    GLState->setBlendMode(true);
    GLState->setBlendFunc(BlendFunction::AdditiveAlpha);

    GLState->setDepthTest(false);

    updateData();

    _lightingParameterSet->bind();
    _renderer->getShaderManager()->bindProgram(GL3ShaderType::LightingPass);

    for (auto& light : _lights) {
        if (light->hasShadow())
        {
            light->updateShadowMapDescriptor(_shadowMapDescriptor);
        }

        GL3Light::LightParameters lightParams;
        light->setParameters(&lightParams);
        _lightUniformBuffer->updateData(&lightParams, 0, sizeof(lightParams), UpdateFlags::DiscardOldData);

        if (light->type == LightType::Point) {
            _renderer->getGLUtil()->drawSphere();
        } else {
            _renderer->getGLUtil()->drawFullScreenTri();
        }
    }

    _lightingParameterSet->unbind();

    // Only try to copy the depth buffer if there actually is one
    auto currentRenderTarget = _renderer->getGLRenderTargetManager()->getCurrentRenderTarget();
    if (currentRenderTarget->hasDepthBuffer())
    {
        auto width = currentRenderTarget->getWidth();
        auto height = currentRenderTarget->getHeight();

        // Now copy the depth component back to the screen
        GLState->Framebuffer.pushBinding();

        GLState->Framebuffer.bindRead(_renderFrameBuffer);
        glBlitFramebuffer(0, 0, (GLint)width, (GLint)height, 0, 0, (GLint)width, (GLint)height,
            GL_DEPTH_BUFFER_BIT, GL_NEAREST);

        GLState->Framebuffer.popBinding();
    }
}

void GL3LightingManager::freeResources() {
    if (glIsTexture(_gBufferTextures[0])) {
        GLState->Texture.unbindAll();
        glDeleteTextures(NUM_GBUFFERS, _gBufferTextures);
        memset(_gBufferTextures, 0, sizeof(_gBufferTextures));
    }

    if (glIsRenderbuffer(_depthRenderBuffer)) {
        glDeleteRenderbuffers(1, &_depthRenderBuffer);
        _depthRenderBuffer = 0;
    }

    if (glIsFramebuffer(_renderFrameBuffer)) {
        glDeleteFramebuffers(1, &_renderFrameBuffer);
        _renderFrameBuffer = 0;
    }
}

void GL3LightingManager::updateData()
{
    auto currentRenderTarget = _renderer->getGLRenderTargetManager()->getCurrentRenderTarget();
    auto width = currentRenderTarget->getWidth();
    auto height = currentRenderTarget->getHeight();

    GlobalParameters params;
    params.uv_scale = glm::vec2((float)width, (float)height) / glm::vec2(_framebufferSize);
    params.window_size = glm::vec2(width, height);
    _globalUniformBuffer->updateData(&params, 0, sizeof(params), UpdateFlags::DiscardOldData);
}

void GL3LightingManager::createFrameBuffer(int width, int height) {
    GLState->Framebuffer.pushBinding();

    _framebufferSize = glm::ivec2(width, height);

    glGenTextures(NUM_GBUFFERS, _gBufferTextures);
    GLState->Texture.unbindAll();

    GLState->Texture.bindTexture(GL_TEXTURE_2D, _gBufferTextures[POSITION_BUFFER]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _framebufferSize.x, _framebufferSize.y, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    GLState->Texture.bindTexture(GL_TEXTURE_2D, _gBufferTextures[NORMAL_BUFFER]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _framebufferSize.x, _framebufferSize.y, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    GLState->Texture.bindTexture(GL_TEXTURE_2D, _gBufferTextures[ALBEDO_BUFFER]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _framebufferSize.x, _framebufferSize.y, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    GLState->Texture.bindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &_depthRenderBuffer);
    GLState->bindRenderBuffer(_depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _framebufferSize.x, _framebufferSize.y);
    GLState->bindRenderBuffer(0);

    glGenFramebuffers(1, &_renderFrameBuffer);
    GLState->Framebuffer.bind(_renderFrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _gBufferTextures[POSITION_BUFFER], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _gBufferTextures[NORMAL_BUFFER], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _gBufferTextures[ALBEDO_BUFFER], 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderBuffer);

    GLuint attachments[NUM_GBUFFERS] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2
    };
    glDrawBuffers(NUM_GBUFFERS, attachments);

    checkFrameBufferStatus();

    _lightingParameterSet->getDescriptor(GL3DescriptorSetPart::LightingSet_AlbedoTexture)
        ->setGLTexture(GL3TextureHandle(GL_TEXTURE_2D, _gBufferTextures[ALBEDO_BUFFER]));

    _lightingParameterSet->getDescriptor(GL3DescriptorSetPart::LightingSet_NormalTexture)
        ->setGLTexture(GL3TextureHandle(GL_TEXTURE_2D, _gBufferTextures[NORMAL_BUFFER]));

    _lightingParameterSet->getDescriptor(GL3DescriptorSetPart::LightingSet_PositionTexture)
        ->setGLTexture(GL3TextureHandle(GL_TEXTURE_2D, _gBufferTextures[POSITION_BUFFER]));

    GLState->Framebuffer.popBinding();
}

void GL3LightingManager::resizeFramebuffer(uint32_t width, uint32_t height) {
    // First check if the new size is actually bigger than the old
    if (width <= _framebufferSize.x && height <= _framebufferSize.y) {
        return; // Framebuffer is big enough, no need to change the size
    }

    freeResources();

    createFrameBuffer(width, height);
}
void GL3LightingManager::changeShadowQuality(SettingsLevel level) {
    switch (level) {
        case SettingsLevel::Disabled:
            _shadowMapResolution = 0;
            break;
        case SettingsLevel::Low:
            _shadowMapResolution = 512;
            break;
        case SettingsLevel::Medium:
            _shadowMapResolution = 1024;
            break;
        case SettingsLevel::High:
            _shadowMapResolution = 2048;
            break;
        case SettingsLevel::Ultra:
            _shadowMapResolution = 4096;
            break;
    }

    for (auto& light : _lights) {
        if (light->hasShadow()) {
            light->changeShadowMapResolution(_shadowMapResolution);
        }
    }
}

GL3Light::GL3Light(GL3Renderer* renderer, GL3LightingManager* manager, LightType in_type, uint32_t depthMapResolution)
    : GL3Object(renderer), _lightingManager(manager), _depthMapResolution(depthMapResolution), _depthTexture(GL_TEXTURE_2D, 0), _depthFrameBuffer(0),
      type(in_type) {
    if (!hasShadow()) {
        return;
    }

    createDepthBuffer(depthMapResolution);


    PipelineProperties props;
    props.blendFunction = BlendFunction::None;
    props.blending = false;

    props.depthFunction = DepthFunction::Less;
    props.depthMode = DepthMode::ReadWrite;

    props.shaderType = ShaderType::ShadowMesh;

    _shadowPipelineState = _renderer->createPipelineState(props);
}
GL3Light::~GL3Light() {
    freeResources();
}

void GL3Light::setPosition(const glm::vec3& pos) {
    position = pos;
    _lightingManager->markDirty();
}

void GL3Light::setDirection(const glm::vec3& dir) {
    direction = glm::normalize(dir);
    _lightingManager->markDirty();
}

void GL3Light::setColor(const glm::vec3& color) {
    this->color = color;
    _lightingManager->markDirty();
}
ShadowMatrices GL3Light::beginShadowPass() {
    Assertion(hasShadow(), "beginShadowPass() called for non-shadowed light!");

    GLState->Framebuffer.pushBinding();
    GLState->Framebuffer.bind(_depthFrameBuffer);

    GLState->setDepthMask(true);
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, _depthMapResolution, _depthMapResolution);

    GLfloat near_plane = 1.0f, far_plane = 70.f;
    _matrices.projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    _matrices.view = glm::lookAt(this->direction * 10.f,
                                 glm::vec3(0.0f, 0.0f, 0.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));

    _shadowPipelineState->bind();

    return _matrices;
}

void GL3Light::endShadowPass() {
    Assertion(hasShadow(), "beginShadowPass() called for non-shadowed light!");

    GLState->Framebuffer.popBinding();
}

void GL3Light::updateShadowMapDescriptor(GL3Descriptor* desc)
{
    desc->setGLTexture(_depthTexture);
}

void GL3Light::changeShadowMapResolution(uint32_t resolution) {
    Assertion(hasShadow(), "Shadow map resolution changed for non-shadowed light!");

    freeResources();
    createDepthBuffer(resolution);
}
void GL3Light::freeResources() {
    GLState->Texture.unbindAll();
    _depthTexture = GL3OwnedTextureHandle(GL_TEXTURE_2D, 0);
    if (glIsFramebuffer(_depthFrameBuffer)) {
        glDeleteFramebuffers(1, &_depthFrameBuffer);
        _depthFrameBuffer = 0;
    }
}
void GL3Light::createDepthBuffer(uint32_t resolution) {
    _depthMapResolution = resolution;

    // Setup shadow mapping
    glGenFramebuffers(1, &_depthFrameBuffer);

    GLuint handle;
    glGenTextures(1, &handle);
    _depthTexture = GL3OwnedTextureHandle(GL_TEXTURE_2D, handle);
    _depthTexture.bind(0);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_DEPTH_COMPONENT,
                 _depthMapResolution,
                 _depthMapResolution,
                 0,
                 GL_DEPTH_COMPONENT,
                 GL_FLOAT,
                 NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
    _depthTexture.unbind(0);

    GLState->Framebuffer.pushBinding();
    GLState->Framebuffer.bind(_depthFrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthTexture.getGLHandle(), 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    checkFrameBufferStatus();

    GLState->Framebuffer.popBinding();
}
void GL3Light::setParameters(LightParameters* params) {
    switch(type)
    {
    case LightType::Directional:
        params->light_type = 0;
        params->light_vector = direction;
        break;
    case LightType::Point:
    {
        params->light_type = 1;
        params->light_vector = position;

        const float cutoff = 0.01f; // Cutoff value after which this light does not affect anything anymore
                                    // This depends on the formula in the shader!
        auto scale = sqrtf((glm::length(color) / cutoff) - 1.f) * 1.2f;
        glm::mat4 lightSphere;
        lightSphere = glm::translate(lightSphere, position);
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

    params->light_color = color;
    params->light_has_shadow = hasShadow() ? GL_TRUE : GL_FALSE;

    if (hasShadow())
    {
        params->light_view_proj_matrix = _matrices.projection * _matrices.view;
    }
}

