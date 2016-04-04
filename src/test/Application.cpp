#include "Application.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

using namespace glm;

namespace {
    struct VertexData {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 tex_coord;
    };

    std::vector<VertexData> getQuadVertexData() {
        std::vector<VertexData> data;
        data.push_back({glm::vec3(-1.0f, -1.0f, 0.f),
                        glm::vec3(0.f, 0.f, -1.f),
                        glm::vec2(0.f, 0.f)
                       });
        data.push_back({glm::vec3(1.0f, -1.0f, 0.f),
                        glm::vec3(0.f, 0.f, -1.f),
                        glm::vec2(1.f, 0.f)
                       });
        data.push_back({glm::vec3(-1.0f, 1.0f, 0.f),
                        glm::vec3(0.f, 0.f, -1.f),
                        glm::vec2(0.f, 1.f)
                       });

        data.push_back({glm::vec3(-1.0f, 1.0f, 0.f),
                        glm::vec3(0.f, 0.f, -1.f),
                        glm::vec2(0.f, 1.f)
                       });
        data.push_back({glm::vec3(1.0f, -1.0f, 0.f),
                        glm::vec3(0.f, 0.f, -1.f),
                        glm::vec2(1.f, 0.f)
                       });
        data.push_back({glm::vec3(1.0f, 1.0f, 0.f),
                        glm::vec3(0.f, 0.f, -1.f),
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

    int width, height;
    SDL_GL_GetDrawableSize(SDL_GL_GetCurrentWindow(), &width, &height);

    _viewMx = glm::translate(mat4(), -glm::vec3(0.0f, 0.0f, 180.0f));
    _modelMx = mat4();

    _projMx = glm::perspectiveFov(45.0f, (float)width, (float)height, 0.01f, 50000.0f);

    auto light = renderer->getLightingManager()->addLight(LightType::Point);
    light->setColor(glm::vec3(1.f, 1.f, 1.f));
    light->setPosition(glm::vec3(10.f, 0.f, 0.f));
    light->setIntesity(200.f);

    light = renderer->getLightingManager()->addLight(LightType::Point);
    light->setColor(glm::vec3(0.f, 1.f, 1.f));
    light->setPosition(glm::vec3(-10.f, 0.f, 0.f));
    light->setIntesity(200.f);

    _renderTarget = renderer->getRenderTargetManager()->createRenderTarget(2048, 2048);

    auto quadData = getQuadVertexData();
    _quadObject = renderer->createBuffer(BufferType::Vertex);
    _quadObject->setData(quadData.data(), sizeof(VertexData) * quadData.size(), BufferUsage::Static);

    _quadLayout = renderer->createVertexLayout();
    auto index = _quadLayout->attachBufferObject(_quadObject.get());
    _quadLayout->addComponent(AttributeType::Position, DataFormat::Vec3, sizeof(VertexData), index,
        offsetof(VertexData, position));
    _quadLayout->addComponent(AttributeType::Normal, DataFormat::Vec3, sizeof(VertexData), index,
        offsetof(VertexData, normal));
    _quadLayout->addComponent(AttributeType::TexCoord, DataFormat::Vec2, sizeof(VertexData), index,
        offsetof(VertexData, tex_coord));
    _quadLayout->finalize();

    PipelineProperties pipeline_props;
    pipeline_props.blendFunction = BlendFunction::None;
    pipeline_props.blending = false;
    pipeline_props.depth_test = false;
    pipeline_props.depthFunction = DepthFunction::Always;
    pipeline_props.shaderType = ShaderType::Mesh;
    _quadPipelineState = renderer->createPipelineState(pipeline_props);

    DrawCallProperties props;
    props.state = _quadPipelineState.get();
    props.vertexLayout = _quadLayout.get();
    _quadDrawCall = renderer->getDrawCallManager()->createDrawCall(props, PrimitiveType::Triangle, 0, 6);
    _quadDrawCall->getParameters()->setMat4(ShaderParameterType::ProjectionMatrix, _projMx);
    _quadDrawCall->getParameters()->setMat4(ShaderParameterType::ModelMatrix, mat4());

    _copyTexture = renderer->createTexture();
}

Application::~Application() {
}

void Application::render(Renderer *renderer) {
    float radius = 5.0f;
    float camX = sin(_timing->getTotalTime()) * radius;
    float camZ = cos(_timing->getTotalTime()) * radius;
    _viewMx = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

    _quadDrawCall->getParameters()->setTexture(ShaderParameterType::ColorTexture, _copyTexture.get());

    /*
    renderer->getRenderTargetManager()->useRenderTarget(_renderTarget.get());
    renderer->clear(glm::vec4(0.f, 1.f, 0.f, 1.f));

    renderer->getLightingManager()->beginLightPass();
    _model->drawModel(renderer, _projMx, _viewMx, _modelMx);
    renderer->getLightingManager()->endLightPass();

    renderer->getRenderTargetManager()->useRenderTarget(nullptr);

    _renderTarget->copyToTexture(_copyTexture.get());
     */

    renderer->clear(glm::vec4(0.f, 0.f, 0.f, 1.f));

    _quadDrawCall->getParameters()->setMat4(ShaderParameterType::ProjectionMatrix, _projMx);
    _quadDrawCall->getParameters()->setMat4(ShaderParameterType::ViewMatrix, _viewMx);
    _quadDrawCall->draw();

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

