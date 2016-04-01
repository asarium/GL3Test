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
        data.push_back({glm::vec3(1.0f, -1.0f, 0.f),
                        glm::vec2(1.f, 0.f)
                       });
        data.push_back({glm::vec3(-1.0f, 1.0f, 0.f),
                        glm::vec2(0.f, 1.f)
                       });

        data.push_back({glm::vec3(-1.0f, 1.0f, 0.f),
                        glm::vec2(0.f, 1.f)
                       });
        data.push_back({glm::vec3(1.0f, -1.0f, 0.f),
                        glm::vec2(1.f, 0.f)
                       });
        data.push_back({glm::vec3(1.0f, 1.0f, 0.f),
                        glm::vec2(1.f, 1.f)
                       });

        return data;
    }
}

GL3LightingManager::GL3LightingManager(GL3Renderer *renderer) : _renderer(renderer), _renderFrameBuffer(0),
                                                                _gBufferTextures{0} {
}

GL3LightingManager::~GL3LightingManager() {
    if (glIsFramebuffer(_renderFrameBuffer)) {
        glDeleteFramebuffers(1, &_renderFrameBuffer);
    }

    if (glIsTexture(_gBufferTextures[0])) {
        glDeleteTextures(NUM_GBUFFERS, _gBufferTextures);
    }
}

bool GL3LightingManager::initialize(int width, int height) {
    glGenFramebuffers(1, &_renderFrameBuffer);
    GLState->bindFramebuffer(_renderFrameBuffer);

    glGenTextures(NUM_GBUFFERS, _gBufferTextures);

    GLState->Texture.bindTexture(GL_TEXTURE_2D, _gBufferTextures[POSITION_BUFFER]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _gBufferTextures[POSITION_BUFFER], 0);

    GLState->Texture.bindTexture(GL_TEXTURE_2D, _gBufferTextures[NORMAL_BUFFER]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _gBufferTextures[NORMAL_BUFFER], 0);

    GLState->Texture.bindTexture(GL_TEXTURE_2D, _gBufferTextures[ALBEDO_BUFFER]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _gBufferTextures[ALBEDO_BUFFER], 0);

    GLState->Texture.bindTexture(GL_TEXTURE_2D, _gBufferTextures[DEPTH_BUFFER]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _gBufferTextures[DEPTH_BUFFER], 0);

    GLState->Texture.bindTexture(GL_TEXTURE_2D, 0);

    GLuint attachments[NUM_GBUFFERS - 1] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(NUM_GBUFFERS - 1, attachments);

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

    _lightingPassParameters.set2dTextureHandle(GL3ShaderParameterType::PositionTexture,
                                               _gBufferTextures[POSITION_BUFFER]);
    _lightingPassParameters.set2dTextureHandle(GL3ShaderParameterType::NormalTexture, _gBufferTextures[NORMAL_BUFFER]);
    _lightingPassParameters.set2dTextureHandle(GL3ShaderParameterType::AlbedoTexture, _gBufferTextures[ALBEDO_BUFFER]);

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
    GLState->bindFramebuffer(_renderFrameBuffer);

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GL3LightingManager::endLightPass() {
    GLState->bindFramebuffer(0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    GLState->setBlendMode(true);
    GLState->setBlendFunc(BlendFunction::Additive);
    GLState->setDepthTest(false);

    for (auto &light : _lights) {

        _lightingPassParameters.setInteger(GL3ShaderParameterType::LightType, light->type == LightType::Point ? 0 : 1);
        _lightingPassParameters.setVec3(GL3ShaderParameterType::LightVectorParameter,
                                        light->type == LightType::Point ? light->position : light->direction);
        _lightingPassParameters.setVec3(GL3ShaderParameterType::LightColor, light->color);
        _lightingPassParameters.setFloat(GL3ShaderParameterType::LightIntensitiy, light->intensity);

        _lightingPassProgram->bindAndSetParameters(&_lightingPassParameters);
        _quadVertexLayout->bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
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
