#define _USE_MATH_DEFINES

#include <math.h>

#include "Application.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <ctime>

#include "util/textures.hpp"

using namespace glm;

namespace {
    struct VertexData {
        glm::vec3 position;
        glm::vec2 tex_coord;
    };

    std::vector<VertexData> getQuadData() {
        std::vector<VertexData> data;
        data.push_back({glm::vec3(-1.0f, -1.0f, 0.f),
                        glm::vec2(0.f, 0.f)
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

Application::Application(Renderer *renderer, Timing *time) {
    _timing = time;
    _renderer = renderer;

    _model.reset(new AssimpModel());
    _model->loadModel(renderer, "resources/duck.dae");

    _particleTexture = util::load_texture(renderer, "resources/Capparticles_0000.png");

    std::srand((unsigned int) std::time(nullptr));
    for (size_t i = 0; i < 3; ++i) {
        float f1 = (float) rand() / (float) RAND_MAX;
        float f2 = (float) rand() / (float) RAND_MAX;
        float f3 = (float) rand() / (float) RAND_MAX;

        Particle p;
        p.position = glm::vec3(f1 * 8.f - 4.f, 0.f, f2 * 8.f - 4.f);

        p.radius = f3 * 5.f - 2.5f;
        p.position.y = p.radius;

        _particles.push_back(p);

        auto light = renderer->getLightingManager()->addLight(LightType::Point);
        light->setIntesity(5.f);
        light->setColor(glm::normalize(p.position));
        light->setPosition(p.position);
        _particleLights.push_back(light);
    }

    _particleBuffer = renderer->createBuffer(BufferType::Vertex);
    _particleBuffer->setData(_particles.data(), sizeof(Particle) * _particles.size(), BufferUsage::Streaming);

    _particleQuadBuffer = renderer->createBuffer(BufferType::Vertex);
    auto quadData = getQuadData();
    _particleQuadBuffer->setData(quadData.data(), quadData.size() * sizeof(VertexData), BufferUsage::Static);

    _particleQuadLayout = renderer->createVertexLayout();
    auto quadBufferIdx = _particleQuadLayout->attachBufferObject(_particleQuadBuffer.get());
    _particleQuadLayout->addComponent(AttributeType::Position, DataFormat::Vec3, sizeof(VertexData), quadBufferIdx,
                                      offsetof(VertexData, position));
    _particleQuadLayout->addComponent(AttributeType::TexCoord, DataFormat::Vec2, sizeof(VertexData), quadBufferIdx,
                                      offsetof(VertexData, tex_coord));

    auto infoBufferIdx = _particleQuadLayout->attachBufferObject(_particleBuffer.get());
    _particleQuadLayout->addInstanceComponent(AttributeType::PositionOffset, DataFormat::Vec3, 1, sizeof(Particle),
                                              infoBufferIdx,
                                              offsetof(Particle, position));
    _particleQuadLayout->addInstanceComponent(AttributeType::Radius, DataFormat::Float, 1, sizeof(Particle),
                                              infoBufferIdx,
                                              offsetof(Particle, radius));

    _particleQuadLayout->finalize();

    PipelineProperties pipelineProperties;
    pipelineProperties.shaderType = ShaderType::InstancedSprite;

    pipelineProperties.blendFunction = BlendFunction::Additive;
    pipelineProperties.blending = true;

    pipelineProperties.depthMode = DepthMode::Read;
    pipelineProperties.depthFunction = DepthFunction::Less;
    _particleQuadPipelineState = renderer->createPipelineState(pipelineProperties);

    DrawCallProperties drawCallProperties;
    drawCallProperties.state = _particleQuadPipelineState.get();
    drawCallProperties.vertexLayout = _particleQuadLayout.get();
    _particleQuadDrawCall = renderer->getDrawCallManager()->createInstancedDrawCall(drawCallProperties,
                                                                                    PrimitiveType::TriangleStrip, 0,
                                                                                    quadData.size());
    _particleQuadDrawCall->getParameters()->setTexture(ShaderParameterType::ColorTexture, _particleTexture.get());

    int width, height;
    SDL_GL_GetDrawableSize(SDL_GL_GetCurrentWindow(), &width, &height);

    _viewMx = glm::translate(mat4(), -glm::vec3(0.0f, 0.0f, 180.0f));
    _modelMx = mat4();

    _projMx = glm::perspectiveFov(45.0f, (float) width, (float) height, 0.01f, 50000.0f);

    _geometryCategory = _renderer->getProfiler()->createCategory("Geometry submit");
    _endLightPassCategory = _renderer->getProfiler()->createCategory("End light pass");
    _particleCategory = _renderer->getProfiler()->createCategory("Particles");
}

Application::~Application() {
}

void Application::render(Renderer *renderer) {
    renderer->clear(glm::vec4(0.f, 0.f, 0.f, 1.f));

    float radius = 10.0f;
    float camX = sin(_timing->getTotalTime()) * radius;
    float camZ = cos(_timing->getTotalTime()) * radius;
    _viewMx = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

    renderer->clear(glm::vec4(0.f, 0.f, 0.f, 1.f));

    renderer->getLightingManager()->beginLightPass(_projMx, _viewMx);
    _geometryCategory->begin();
    _model->drawModel(renderer, _projMx, _viewMx, _modelMx);
    _geometryCategory->end();

    _endLightPassCategory->begin();
    renderer->getLightingManager()->endLightPass();
    _endLightPassCategory->end();

    _particleCategory->begin();
    updateParticles();
    _particleQuadDrawCall->drawInstanced(_particles.size());
    _particleCategory->end();

    renderer->presentNextFrame();

    auto profileResults = _renderer->getProfiler()->getResults();
//    for (auto& result : profileResults) {
//        printf("%s: %3.3fms - %3.3fms\n", result.name, (double) result.cpu_time / 1000000.f, (double) result.gpu_time / 1000000.f);
//    }
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
                    auto settings = _renderer->getSettingsManager()->getCurrentSettings();
                    settings.vertical_sync = _last_vsync;
                    _renderer->getSettingsManager()->changeSettings(settings);
                    break;
            }
            break;
    }
}

void Application::changeResolution(uint32_t width, uint32_t height) {
    auto settings = _renderer->getSettingsManager()->getCurrentSettings();
    settings.resolution = glm::uvec2(width, height);
    _renderer->getSettingsManager()->changeSettings(settings);

    _projMx = glm::perspectiveFov(45.0f, (float) width, (float) height, 0.01f, 50000.0f);
}

void Application::updateParticles() {
    size_t index = 0;
    for (auto &particle : _particles) {
        float time = _timing->getTotalTime() + particle.radius;

        float y = (float) std::sin(time * M_PI_2);
        particle.position.y = y;
        _particleLights[index]->setPosition(particle.position);

        ++index;
    }
    _particleBuffer->updateData(_particles.data(), sizeof(Particle) * _particles.size(), 0,
                                UpdateFlags::DiscardOldData);

    _particleQuadDrawCall->getParameters()->setMat4(ShaderParameterType::ProjectionMatrix, _projMx);
    _particleQuadDrawCall->getParameters()->setMat4(ShaderParameterType::ViewMatrix, _viewMx);
}

