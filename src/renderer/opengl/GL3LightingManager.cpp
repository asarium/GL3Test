//
//

#include "GL3LightingManager.hpp"
#include "GL3Renderer.hpp"
#include "GL3State.hpp"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <util/Assertion.hpp>

GL3LightingManager::GL3LightingManager(GL3Renderer* renderer) : GL3Object(renderer), _renderFrameBuffer(0),
                                                                _gBufferTextures{ 0 }, _shadowMapResolution(0) {
}

GL3LightingManager::~GL3LightingManager() {
    freeResources();
}

bool GL3LightingManager::initialize() {
    _lightingPassProgram = _renderer->getShaderManager()->getShader(GL3ShaderType::LightingPass);

    PipelineProperties pipelineProperties;
    pipelineProperties.shaderType = ShaderType::LightedMesh;

    pipelineProperties.depthMode = DepthMode::ReadWrite;
    pipelineProperties.depthFunction = DepthFunction::Less;

    pipelineProperties.blending = false;
    pipelineProperties.blendFunction = BlendFunction::None;

    _geometryPipelineState = _renderer->createPipelineState(pipelineProperties);

    return true;
}

Light* GL3LightingManager::addLight(LightType type, bool shadows) {
    if (shadows && type != LightType::Directional) {
        return nullptr; // Shadows are only supported for directional lights currently
    }

    _lights.emplace_back(new GL3Light(this->_renderer, this, type, shadows ? _shadowMapResolution : 0));
    return _lights.back().get();
}

void GL3LightingManager::removeLight(Light* light) {
    for (auto iter = _lights.begin(); iter != _lights.end(); ++iter) {
        if (iter->get() == light) {
            _lights.erase(iter);
            break;
        }
    }
}

void GL3LightingManager::clearLights() {
    _lights.clear();
}

void GL3LightingManager::beginLightPass(const glm::mat4& projection, const glm::mat4& view) {
    _projectionMatrix = projection;
    _viewMatrix = view;

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

    auto currentRenderTarget = _renderer->getGLRenderTargetManager()->getCurrentRenderTarget();
    auto width = currentRenderTarget->getWidth();
    auto height = currentRenderTarget->getHeight();

    glm::vec2 uv_scale = glm::vec2((float) width, (float) height) / glm::vec2(_framebufferSize);
    _lightingPassParameters.setVec2(GL3ShaderParameterType::UVScale, uv_scale);
    _lightingPassParameters.setMat4(GL3ShaderParameterType::ProjectionMatrix, _projectionMatrix);
    _lightingPassParameters.setMat4(GL3ShaderParameterType::ViewMatrix, _viewMatrix);
    _lightingPassParameters.setVec2(GL3ShaderParameterType::WindowSize, glm::vec2(width, height));
    _lightingPassParameters.setInteger(GL3ShaderParameterType::LightHasShadow, 0);

    for (auto& light : _lights) {
        switch (light->type) {
            case LightType::Point: {
                _lightingPassParameters.setInteger(GL3ShaderParameterType::LightType, 0);
                _lightingPassParameters.setVec3(GL3ShaderParameterType::LightVectorParameter, light->position);

                const float cutoff = 0.01f; // Cutoff value after which this light does not affect anything anymore
                // This depends on the formula in the shader!
                auto scale = sqrtf((glm::length(light->color) / cutoff) - 1.f) * 1.2f;
                glm::mat4 lightSphere;
                lightSphere = glm::translate(lightSphere, light->position);
                lightSphere = glm::scale(lightSphere, glm::vec3(scale));

                _lightingPassParameters.setMat4(GL3ShaderParameterType::ModelMatrix, lightSphere);
                break;
            }
            case LightType::Directional:
                _lightingPassParameters.setInteger(GL3ShaderParameterType::LightType, 1);
                _lightingPassParameters.setVec3(GL3ShaderParameterType::LightVectorParameter, light->direction);

                if (light->hasShadow()) {
                    light->setParameters(&_lightingPassParameters);
                }
                break;
            case LightType::Ambient:
                _lightingPassParameters.setInteger(GL3ShaderParameterType::LightType, 2);
                break;
        }

        _lightingPassParameters.setVec3(GL3ShaderParameterType::LightColor, light->color);

        _lightingPassProgram->bindAndSetParameters(&_lightingPassParameters);

        if (light->type == LightType::Point) {
            _renderer->getGLUtil()->drawSphere();
        } else {
            _renderer->getGLUtil()->drawFullScreenTri();
        }
    }

    // Only try to copy the depth buffer if there actually is one
    if (currentRenderTarget->hasDepthBuffer())
    {
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

    _lightingPassParameters.set2dTextureHandle(GL3ShaderParameterType::PositionTexture,
                                               _gBufferTextures[POSITION_BUFFER]);
    _lightingPassParameters.set2dTextureHandle(GL3ShaderParameterType::NormalTexture, _gBufferTextures[NORMAL_BUFFER]);
    _lightingPassParameters.set2dTextureHandle(GL3ShaderParameterType::AlbedoTexture, _gBufferTextures[ALBEDO_BUFFER]);

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
    : GL3Object(renderer), _lightingManager(manager), type(in_type), _depthTexture(0), _depthFrameBuffer(0),
      _depthMapResolution(depthMapResolution) {
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
}

void GL3Light::setDirection(const glm::vec3& dir) {
    direction = glm::normalize(dir);
}

void GL3Light::setColor(const glm::vec3& color) {
    this->color = color;
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
void GL3Light::changeShadowMapResolution(uint32_t resolution) {
    Assertion(hasShadow(), "Shadow map resolution changed for non-shadowed light!");

    freeResources();
    createDepthBuffer(resolution);
}
void GL3Light::freeResources() {
    if (glIsTexture(_depthTexture)) {
        glDeleteTextures(1, &_depthTexture);
        _depthTexture = 0;
    }
    if (glIsFramebuffer(_depthFrameBuffer)) {
        glDeleteFramebuffers(1, &_depthFrameBuffer);
        _depthFrameBuffer = 0;
    }
}
void GL3Light::createDepthBuffer(uint32_t resolution) {
    _depthMapResolution = resolution;

    // Setup shadow mapping
    glGenFramebuffers(1, &_depthFrameBuffer);

    glGenTextures(1, &_depthTexture);
    GLState->Texture.bindTexture(0, GL_TEXTURE_2D, _depthTexture);
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

    GLState->Framebuffer.pushBinding();
    GLState->Framebuffer.bind(_depthFrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    checkFrameBufferStatus();

    GLState->Framebuffer.popBinding();
}
void GL3Light::setParameters(GL3ShaderParameters* params) {
    params->setInteger(GL3ShaderParameterType::LightHasShadow, 1);
    params->set2dTextureHandle(GL3ShaderParameterType::DirectionalShadowMap, _depthTexture);
    params->setMat4(GL3ShaderParameterType::LightProjectionMatrix, _matrices.projection);
    params->setMat4(GL3ShaderParameterType::LightViewMatrix, _matrices.view);
}

