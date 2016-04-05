//
//

#include "GL3LightingManager.hpp"
#include "GL3Renderer.hpp"
#include "GL3State.hpp"

#include <iostream>

namespace {
    struct VertexData {
        glm::vec3 position;
        glm::vec2 tex_coord;
    };

    std::vector<VertexData> getQuadVertexData() {
        std::vector<VertexData> data;
        data.push_back({glm::vec3(-1.0f, -1.0f, 0.f),
                        glm::vec2(0.f, 0.f)
                       });
        data.push_back({glm::vec3(3.0f, -1.0f, 0.f),
                        glm::vec2(2.f, 0.f)
                       });
        data.push_back({glm::vec3(-1.0f, 3.0f, 0.f),
                        glm::vec2(0.f, 2.f)
                       });

        return data;
    }

    int next_power_of_two(int x) {
        if (x < 0)
            return 0;
        --x;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        return x + 1;
    }
}

GL3LightingManager::GL3LightingManager(GL3Renderer *renderer) : GL3Object(renderer), _renderFrameBuffer(0),
                                                                _gBufferTextures{0} {
}

GL3LightingManager::~GL3LightingManager() {
    freeResources();
}

bool GL3LightingManager::initialize() {
    _quadVertexBuffer = _renderer->createBuffer(BufferType::Vertex);
    auto quadData = getQuadVertexData();
    _quadVertexBuffer->setData(quadData.data(), quadData.size() * sizeof(VertexData), BufferUsage::Static);

    auto layout = _renderer->createVertexLayout();
    auto vertexBufferIndex = layout->attachBufferObject(_quadVertexBuffer.get());
    layout->addComponent(AttributeType::Position, DataFormat::Vec3, sizeof(VertexData), vertexBufferIndex,
                         offsetof(VertexData, position));
    layout->addComponent(AttributeType::TexCoord, DataFormat::Vec2, sizeof(VertexData), vertexBufferIndex,
                         offsetof(VertexData, tex_coord));
    layout->finalize();

    _quadVertexLayout.reset(static_cast<GL3VertexLayout *>(layout.release()));

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

Light *GL3LightingManager::addLight(LightType type) {
    _lights.emplace_back(new GL3Light(type));
    return _lights.back().get();
}

void GL3LightingManager::removeLight(Light *light) {
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

void GL3LightingManager::beginLightPass() {
    auto currentRenderTarget = _renderer->getGLRenderTargetManager()->getCurrentRenderTarget();
    auto width = currentRenderTarget->getWidth();
    auto height = currentRenderTarget->getHeight();

    // Make sure our framebuffer is big enough for the current render target
    resizeFramebuffer((uint32_t) width, (uint32_t) height);

    GLState->Framebuffer.pushBinding();
    GLState->Framebuffer.bind(_renderFrameBuffer);

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

    for (auto &light : _lights) {
        _lightingPassParameters.setInteger(GL3ShaderParameterType::LightType, light->type == LightType::Point ? 0 : 1);
        _lightingPassParameters.setVec3(GL3ShaderParameterType::LightVectorParameter,
                                        light->type == LightType::Point ? light->position : light->direction);
        _lightingPassParameters.setVec3(GL3ShaderParameterType::LightColor, light->color);
        _lightingPassParameters.setFloat(GL3ShaderParameterType::LightIntensitiy, light->intensity);

        _lightingPassProgram->bindAndSetParameters(&_lightingPassParameters);
        _quadVertexLayout->bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    // Now copy the depth component back to the screen
    GLState->Framebuffer.pushBinding();

    GLState->Framebuffer.bindRead(_renderFrameBuffer);
    glBlitFramebuffer(0, 0, (GLint) width, (GLint) height, 0, 0, (GLint) width, (GLint) height,
                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    GLState->Framebuffer.popBinding();
}

GL3Light::GL3Light(LightType in_type) : type(in_type) {

}

void GL3Light::setPosition(const glm::vec3 &pos) {
    position = pos;
}

void GL3Light::setDirection(const glm::vec3 &dir) {
    direction = dir;
}

void GL3Light::setColor(const glm::vec3 &color) {
    this->color = glm::normalize(color);
}

void GL3Light::setIntesity(float intensity) {
    this->intensity = intensity;
}

PipelineState *GL3LightingManager::getRenderPipeline() {
    return _geometryPipelineState.get();
}

void GL3LightingManager::freeResources() {
    if (glIsFramebuffer(_renderFrameBuffer)) {
        glDeleteFramebuffers(1, &_renderFrameBuffer);
        _renderFrameBuffer = 0;
    }

    if (glIsTexture(_gBufferTextures[0])) {
        glDeleteTextures(NUM_GBUFFERS, _gBufferTextures);
        memset(_gBufferTextures, 0, sizeof(_gBufferTextures));
    }

    if (glIsRenderbuffer(_depthRenderBuffer)) {
        glDeleteRenderbuffers(1, &_depthRenderBuffer);
        _depthRenderBuffer = 0;
    }
}

void GL3LightingManager::createFrameBuffer(int width, int height) {
    GLState->Framebuffer.pushBinding();

    _framebufferSize = glm::ivec2(width, height);

    glGenTextures(NUM_GBUFFERS, _gBufferTextures);

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

    GLuint attachments[NUM_GBUFFERS] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(NUM_GBUFFERS, attachments);

    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        switch (status) {
            case GL_FRAMEBUFFER_COMPLETE:
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                std::cout << "FBO error: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"
                << std::endl;
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                std::cout << "FBO error: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"
                << std::endl;
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                std::cout << "FBO error: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"
                << std::endl;
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                std::cout << "FBO error: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"
                << std::endl;
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                std::cout << "FBO error: GL_FRAMEBUFFER_UNSUPPORTED"
                << std::endl;
                break;
            default:
                std::cout << "FBO error: Unknown error"
                << std::endl;
                break;
        }
    }

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

