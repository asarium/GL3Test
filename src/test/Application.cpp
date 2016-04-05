
#define _USE_MATH_DEFINES
#include <math.h>

#include "Application.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "util/textures.hpp"

using namespace glm;

Application::Application(Renderer *renderer, Timing *time) {
    _timing = time;
    _renderer = renderer;

    _model.reset(new AssimpModel());
    _model->loadModel(renderer, "resources/duck.dae");

    _particleTexture = util::load_texture(renderer, "resources/Capparticles_0000.png");

    for (size_t i = 0; i < 10; ++i) {
        float f1 = (float) rand() / (float) RAND_MAX;
        float f2 = (float) rand() / (float) RAND_MAX;
        float f3 = (float) rand() / (float) RAND_MAX;

        Particle p;
        p.position = glm::vec3(f1 * 10.f - 5.f, 0.f, f2 * 10.f - 5.f);

        p.radius = f3 * 5.f - 2.5f;
        p.position.y = p.radius;

        _particles.push_back(p);
    }

    _particleBuffer = renderer->createBuffer(BufferType::Vertex);
    _particleBuffer->setData(_particles.data(), sizeof(Particle) * _particles.size(), BufferUsage::Streaming);

    _particleLayout = renderer->createVertexLayout();
    auto pBufferIdx = _particleLayout->attachBufferObject(_particleBuffer.get());
    _particleLayout->addComponent(AttributeType::Position, DataFormat::Vec3, sizeof(Particle), pBufferIdx,
                                  offsetof(Particle, position));
    _particleLayout->addComponent(AttributeType::Radius, DataFormat::Float, sizeof(Particle), pBufferIdx,
                                  offsetof(Particle, radius));
    _particleLayout->finalize();

    PipelineProperties pipelineProperties;
    pipelineProperties.shaderType = ShaderType::PointSprite;

    pipelineProperties.blendFunction = BlendFunction::Additive;
    pipelineProperties.blending = true;

    pipelineProperties.depthMode = DepthMode::Read;
    pipelineProperties.depthFunction = DepthFunction::Less;
    _particlePipelineState = renderer->createPipelineState(pipelineProperties);

    DrawCallProperties drawCallProperties;
    drawCallProperties.state = _particlePipelineState.get();
    drawCallProperties.vertexLayout = _particleLayout.get();
    _particleDrawCall = renderer->getDrawCallManager()->createVariableDrawCall(drawCallProperties,
                                                                               PrimitiveType::Point);
    _particleDrawCall->getParameters()->setTexture(ShaderParameterType::ColorTexture, _particleTexture.get());

    int width, height;
    SDL_GL_GetDrawableSize(SDL_GL_GetCurrentWindow(), &width, &height);

    _viewMx = glm::translate(mat4(), -glm::vec3(0.0f, 0.0f, 180.0f));
    _modelMx = mat4();

    _projMx = glm::perspectiveFov(45.0f, (float) width, (float) height, 0.01f, 50000.0f);

    auto light = renderer->getLightingManager()->addLight(LightType::Point);
    light->setColor(glm::vec3(1.f, 1.f, 1.f));
    light->setPosition(glm::vec3(10.f, 0.f, 0.f));
    light->setIntesity(200.f);

    light = renderer->getLightingManager()->addLight(LightType::Point);
    light->setColor(glm::vec3(0.f, 1.f, 1.f));
    light->setPosition(glm::vec3(-10.f, 0.f, 0.f));
    light->setIntesity(200.f);
}

Application::~Application() {
}

void Application::render(Renderer *renderer) {
    renderer->clear(glm::vec4(0.f, 0.f, 0.f, 1.f));

    float radius = 5.0f;
    float camX = sin(_timing->getTotalTime()) * radius;
    float camZ = cos(_timing->getTotalTime()) * radius;
    _viewMx = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

    renderer->clear(glm::vec4(0.f, 1.f, 0.f, 1.f));

    renderer->getLightingManager()->beginLightPass();
    _model->drawModel(renderer, _projMx, _viewMx, _modelMx);
    renderer->getLightingManager()->endLightPass();

    updateParticles();
    _particleDrawCall->draw(_particles.size(), 0);

    renderer->presentNextFrame();
}

void Application::handleEvent(SDL_Event *event) {
    switch (event->type) {
        case SDL_KEYUP:
            switch (event->key.keysym.scancode) {
                case SDL_SCANCODE_R:
                    _resolution_index = (_resolution_index + 1) % 2;

                    if (_resolution_index == 0) {
                        changeResolution(1680, 1050);
                    } else {
                        changeResolution(1920, 1200);
                    }
                    break;
                case SDL_SCANCODE_F:
                    SDL_SetWindowFullscreen(SDL_GL_GetCurrentWindow(), SDL_WINDOW_FULLSCREEN);
                    break;
                case SDL_SCANCODE_B:
                    SDL_SetWindowFullscreen(SDL_GL_GetCurrentWindow(), 0);
                    SDL_SetWindowBordered(SDL_GL_GetCurrentWindow(), SDL_FALSE);
                    break;
                case SDL_SCANCODE_W:
                    SDL_SetWindowFullscreen(SDL_GL_GetCurrentWindow(), 0);
                    SDL_SetWindowBordered(SDL_GL_GetCurrentWindow(), SDL_TRUE);
                    break;
                case SDL_SCANCODE_V:
                    _last_vsync = !_last_vsync;
                    _renderer->getSettings()->setOption(SettingsParameter::VerticalSync, &_last_vsync);
                    break;
            }
            break;
    }
}

void Application::changeResolution(uint32_t width, uint32_t height) {
    _renderer->getSettings()->changeResolution(width, height);

    _projMx = glm::perspectiveFov(45.0f, (float) width, (float) height, 0.01f, 50000.0f);
}

void Application::updateParticles() {
    for (auto &particle : _particles) {
        float time = _timing->getTotalTime() + particle.radius;

        float y = (float) std::sin(time * M_PI_2);
        particle.position.y = y;
    }
    _particleBuffer->updateData(_particles.data(), sizeof(Particle) * _particles.size(), 0,
                                UpdateFlags::DiscardOldData);

    _particleDrawCall->getParameters()->setMat4(ShaderParameterType::ProjectionMatrix, _projMx);
    _particleDrawCall->getParameters()->setMat4(ShaderParameterType::ViewMatrix, _viewMx);
}

