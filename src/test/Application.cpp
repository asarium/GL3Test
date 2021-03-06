#define _USE_MATH_DEFINES

#include <math.h>

#include "Application.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <numeric>
#include <ctime>
#include <model/AssimpModelConverter.hpp>
#include <model/ModelLoader.hpp>
#include <renderer/RenderTargetManager.hpp>

#include "util/textures.hpp"
#include "NanoVGRenderer.hpp"

using namespace glm;

namespace {
struct VertexData {
    glm::vec3 position;
    glm::vec2 tex_coord;
    glm::vec3 normal;
};

std::vector<VertexData> getQuadData() {
    std::vector<VertexData> data;
    data.push_back({
                       glm::vec3(-1.0f, -1.0f, 0.f),
                       glm::vec2(0.f, 0.f),
                       glm::vec3(0.f, 0.f, -1.f)
                   });
    data.push_back({
                       glm::vec3(-1.0f, 1.0f, 0.f),
                       glm::vec2(0.f, 1.f),
                       glm::vec3(0.f, 0.f, -1.f)
                   });
    data.push_back({
                       glm::vec3(1.0f, -1.0f, 0.f),
                       glm::vec2(1.f, 0.f),
                       glm::vec3(0.f, 0.f, -1.f)
                   });
    data.push_back({
                       glm::vec3(1.0f, 1.0f, 0.f),
                       glm::vec2(1.f, 1.f),
                       glm::vec3(0.f, 0.f, -1.f)
                   });

    return data;
}

std::vector<VertexData> getFullscreenTriData() {
    std::vector<VertexData> data;
    data.push_back({
                       glm::vec3(-1.0f, -1.0f, 0.f),
                       glm::vec2(0.f, 0.f),
                       glm::vec3(0.f)
                   });
    data.push_back({
                       glm::vec3(3.0f, -1.0f, 0.f),
                       glm::vec2(2.f, 0.f),
                       glm::vec3(0.f)
                   });
    data.push_back({
                       glm::vec3(-1.0f, 3.0f, 0.f),
                       glm::vec2(0.f, 2.f),
                       glm::vec3(0.f)
                   });

    return data;
}

void drawTimes(NVGcontext* ctx, const std::deque<float>& vals, size_t maxVals, int x, int y, int width, int height,
               const char* text) {
    if (vals.empty()) {
        return;
    }

    nvgSave(ctx);
    nvgReset(ctx);
    nvgFontFace(ctx, "sans");

    nvgFontSize(ctx, 20.f);
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgText(ctx, x + 5, y + 5, text, nullptr);

    auto sum = std::accumulate(vals.begin(), vals.end(), 0.f);
    auto avg = sum / vals.size();

    char str[64];
    sprintf(str, "%.2fms", avg);

    nvgFontSize(ctx, 15.f);
    nvgTextAlign(ctx, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);
    nvgText(ctx, x + width - 3, y + 5, str, nullptr);

    nvgTranslate(ctx, x, y);
    nvgScale(ctx, width, height);

    nvgBeginPath(ctx);
    nvgRect(ctx, 0.f, 0.f, 1.f, 1.f);
    nvgFillColor(ctx, nvgRGBA(128, 128, 128, 128));
    nvgFill(ctx);

    nvgBeginPath(ctx);
    nvgMoveTo(ctx, 0.f, 1.f);

    auto maxTime = *std::max_element(vals.begin(), vals.end());
    if (maxTime < 60.f) {
        // Make 30 ms the minimum scale
        maxTime = 60.f;
    }

    size_t index = 0;
    for (auto time : vals) {
        auto xPos = (float) index / maxVals;
        auto yPos = time / maxTime;

        nvgLineTo(ctx, xPos, 1.f - yPos);

        ++index;
    }
    auto endPos = (float) (vals.size() - 1) / maxVals;
    nvgLineTo(ctx, endPos, 1.f);
    nvgFillColor(ctx, nvgRGBA(255, 192, 0, 128));
    nvgFill(ctx);

    nvgReset(ctx);
}
}

Application::Application(Renderer* renderer, Timing* time, SDL_Window* window)
    : _timing(time), _renderer(renderer), _window(window), _lightingManager(renderer) {
    auto freq = SDL_GetPerformanceFrequency();
    auto begin = SDL_GetPerformanceCounter();
    AssimpModelConverter converter;
    converter.convertModel("resources/duck.dae", "duck", "resources/export");
    auto end = SDL_GetPerformanceCounter();

    printf("Converting: %fms\n", (end - begin) * 1000.0 / freq);

    ModelLoader loader(_renderer);

    begin = SDL_GetPerformanceCounter();
    _model = std::move(loader.loadModel("resources/export/duck"));
    end = SDL_GetPerformanceCounter();

    printf("Loading: %fms\n", (end - begin) * 1000.0 / freq);

    auto modelPipelineState = _lightingManager.getGeometryProperties();
    modelPipelineState.vertexInput = _model->getVertexInputState();
    modelPipelineState.primitive_type = PrimitiveType::Triangle;
    _modelPipelineState = _renderer->createPipelineState(modelPipelineState);

    _nvgCtx = createNanoVGContext(_renderer);

    _floorVertexDataObject = renderer->createBuffer(BufferType::Vertex);
    auto quadData = getQuadData();
    _floorVertexDataObject->setData(quadData.data(), quadData.size() * sizeof(VertexData), BufferUsage::Static);

    auto floorPipelineProperties = _lightingManager.getGeometryProperties();

    VertexInputStateProperties floorVertexInput;
    floorVertexInput.addComponent(AttributeType::Position, 0, DataFormat::Vec3, offsetof(VertexData, position));
    floorVertexInput.addComponent(AttributeType::TexCoord, 0, DataFormat::Vec2, offsetof(VertexData, tex_coord));
    floorVertexInput.addComponent(AttributeType::Normal, 0, DataFormat::Vec3, offsetof(VertexData, normal));

    floorVertexInput.addBufferBinding(0, false, sizeof(VertexData));
    floorPipelineProperties.vertexInput = floorVertexInput;
    floorPipelineProperties.primitive_type = PrimitiveType::TriangleStrip;

    _floorPipelineState = _renderer->createPipelineState(floorPipelineProperties);

    VertexArrayProperties vaoProps;
    vaoProps.addBufferBinding(0, _floorVertexDataObject.get());
    _floorVertexArrayObject = _renderer->createVertexArrayObject(floorVertexInput, vaoProps);

    _floorTexture = util::load_texture(renderer, "resources/wood.png");

    _floorDrawCall.array((uint32_t) quadData.size(), 0);

    auto floorMatrix = mat4();
    floorMatrix = glm::translate(floorMatrix, vec3(0.f, 0.f, 0.f));
    floorMatrix = glm::rotate(floorMatrix, radians(90.f), vec3(1.f, 0.f, 0.f));
    floorMatrix = glm::scale(floorMatrix, vec3(10.f, 10.f, 10.f));

    ModelUniformData data;
    data.model_matrix = floorMatrix;
    data.normal_model_matrix = glm::transpose(glm::inverse(floorMatrix));
    _floorUniformObject = _renderer->createBuffer(BufferType::Uniform);
    _floorUniformObject->setData(&data, sizeof(ModelUniformData), BufferUsage::Static);

    _floorModelDescriptorSet = _renderer->createDescriptorSet(DescriptorSetType::ModelSet);
    _floorModelDescriptorSet->getDescriptor(DescriptorSetPart::ModelSet_Uniforms)->setUniformBuffer(_floorUniformObject.get(),
                                                                                                    0,
                                                                                                    sizeof(data));
    _floorModelDescriptorSet->getDescriptor(DescriptorSetPart::ModelSet_DiffuseTexture)->setTexture(_floorTexture.get());

    _sunLight = _lightingManager.addLight(lighting::LightType::Directional, true);
    _sunLight->setDirection(glm::vec3(10.f, 5.f, 0.f));
    _sunLight->setColor(glm::vec3(1.f));
    _lightingManager.updateLightData();

//    auto light = _renderer->getLightingManager()->addLight(LightType::Point, false);
//    light->setPosition(glm::vec3(-3.f, 1.f, 0.f));
//    light->setColor(glm::vec3(2.4f, 8.2f, 5.3f));

    int width, height;
    SDL_GL_GetDrawableSize(SDL_GL_GetCurrentWindow(), &width, &height);

    _viewUniforms.projection_matrix = glm::perspectiveFov(45.0f, (float) width, (float) height, 0.01f, 50000.0f);

    _wholeFrameCategory = _renderer->getProfiler()->createCategory("Whole frame");
    nvgCreateFont(_nvgCtx, "sans", "resources/Roboto-Regular.ttf");

    _viewUniformBuffer = _renderer->createBuffer(BufferType::Uniform);
    _viewUniformBuffer->setData(nullptr, sizeof(ViewUniformData), BufferUsage::Streaming);

    _viewDescriptorSet = _renderer->createDescriptorSet(DescriptorSetType::ViewSet);
    auto descriptor = _viewDescriptorSet->getDescriptor(DescriptorSetPart::ViewSet_Uniforms);
    descriptor->setUniformBuffer(_viewUniformBuffer.get(), 0, sizeof(ViewUniformData));
}

Application::~Application() {
    deleteNanoVGContext(_nvgCtx);
}

void Application::render(Renderer* renderer) {
    float radius = 3.0f;
    float camX = sin(_timing->getTotalTime()) * radius;
    float camZ = cos(_timing->getTotalTime()) * radius;

    auto cmd = _renderer->createCommandBuffer();

    _wholeFrameCategory->begin();

    cmd->clear(glm::vec4(0.f, 0.f, 0.f, 1.f), ClearTarget::Color | ClearTarget::Depth | ClearTarget::Stencil);

    auto matrices = _sunLight->beginShadowPass(cmd.get(), _viewUniforms);
    ViewUniformData shadowView;
    shadowView.projection_matrix = matrices.projection;
    shadowView.view_matrix = matrices.view;
    shadowView.view_projection_matrix = shadowView.projection_matrix * shadowView.view_matrix;
    _viewUniformBuffer->updateData(&shadowView, 0, sizeof(shadowView), UpdateFlags::DiscardOldData);

    cmd->bindDescriptorSet(_viewDescriptorSet.get());
    renderScene(cmd.get());

    _sunLight->endShadowPass(cmd.get());

    _viewUniforms.view_matrix =
        glm::lookAt(glm::vec3(camX, 3.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    _viewUniforms.view_projection_matrix = _viewUniforms.projection_matrix * _viewUniforms.view_matrix;
    _viewUniformBuffer->updateData(&_viewUniforms, 0, sizeof(_viewUniforms), UpdateFlags::DiscardOldData);

    cmd->bindDescriptorSet(_viewDescriptorSet.get());
    _lightingManager.beginLightPass(cmd.get());

    cmd->bindPipeline(_modelPipelineState);
    renderScene(cmd.get());

    _lightingManager.endLightPass(cmd.get());

    renderUI();

    _wholeFrameCategory->end();
    renderer->presentNextFrame();

    auto results = _renderer->getProfiler()->getResults();
    if (!results.empty()) {
        ProfilingResult& result = results.front();
        _cpuTimes.push_back((float) result.cpu_time / 1000000.f);
        _gpuTimes.push_back((float) result.gpu_time / 1000000.f);

        while (_gpuTimes.size() > 120) {
            // Only keep the 120 most recent times
            _gpuTimes.pop_front();
            _cpuTimes.pop_front();
        }
    }
}


void Application::changeResolution(uint32_t width, uint32_t height) {
    SDL_SetWindowSize(_window, width, height);
    if (SDL_GetWindowFlags(_window) & SDL_WINDOW_FULLSCREEN) {
        // In fullscreen mode the normal method doesn't work
        SDL_DisplayMode target;
        target.w = width;
        target.h = height;
        target.format = 0; // don't care
        target.refresh_rate = 0; // dont't care
        target.driverdata = 0; // initialize to 0

        SDL_DisplayMode closest;

        if (SDL_GetClosestDisplayMode(0, &target, &closest) != nullptr) {
            // I haven't found a perfect solution for changing resolution when in fullscreen mode
            // On Windows simply calling SDL_SetWindowDisplayMode is enough but on Linux (with GNOME) it just doesn't work
            // This workaround works for linux but is suboptimal for Windows

            SDL_SetWindowFullscreen(_window, 0);

            SDL_SetWindowDisplayMode(_window, &closest);

            SDL_Delay(250);

            SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN);
        }
    }

    auto settings = _renderer->getSettingsManager()->getCurrentSettings();
    settings.resolution = glm::uvec2(width, height);
    _renderer->getSettingsManager()->changeSettings(settings);

    _viewUniforms.projection_matrix = glm::perspectiveFov(45.0f, (float) width, (float) height, 0.01f, 50000.0f);
}

void Application::renderUI() {
    auto settings = _renderer->getSettingsManager()->getCurrentSettings();

    nvgBeginFrame(_nvgCtx, settings.resolution.x, settings.resolution.y, 1.f);

    auto w = 300;
    auto h = 50;
    auto x = settings.resolution.x - w - 20;
    auto y = 20;

    drawTimes(_nvgCtx, _cpuTimes, 120, x, y, w, h, "CPU Time");

    y += h + 20;
    drawTimes(_nvgCtx, _gpuTimes, 120, x, y, w, h, "GPU Time");

    nvgEndFrame(_nvgCtx);
}
void Application::renderScene(CommandBuffer* cmd) {
    DEBUG_SCOPE(debug1, _renderer->getDebugging(), "Scene render");

    _model->prepareData(mat4());

    _model->render(cmd);

    cmd->bindDescriptorSet(_floorModelDescriptorSet.get());
    cmd->bindVertexArrayObject(_floorVertexArrayObject);
    cmd->bindPipeline(_floorPipelineState);
    _floorDrawCall.draw(cmd);
    cmd->unbindDescriptorSet(_floorModelDescriptorSet);
}

void Application::handleEvent(SDL_Event* event) {
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
                    SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN);
                    break;
                case SDL_SCANCODE_B:
                    SDL_SetWindowFullscreen(_window, 0);
                    SDL_SetWindowBordered(_window, SDL_FALSE);
                    break;
                case SDL_SCANCODE_W:
                    SDL_SetWindowFullscreen(_window, 0);
                    SDL_SetWindowBordered(_window, SDL_TRUE);
                    break;
                case SDL_SCANCODE_V: {
                    _last_vsync = !_last_vsync;
                    auto settings = _renderer->getSettingsManager()->getCurrentSettings();
                    settings.vertical_sync = _last_vsync;
                    _renderer->getSettingsManager()->changeSettings(settings);
                    break;
                }
                default:
                    break;
            }
            break;
    }
}
