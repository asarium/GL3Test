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

Application::Application(Renderer* renderer, Timing* time)
    : _timing(time), _renderer(renderer), _lightingManager(renderer) {
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

    _floorVertexDataObject = renderer->createBuffer(BufferType::Vertex);
    auto quadData = getQuadData();
    _floorVertexDataObject->setData(quadData.data(), quadData.size() * sizeof(VertexData), BufferUsage::Static);

    _floorVertexLayout = renderer->createVertexLayout();
    auto quadBufferIdx = _floorVertexLayout->attachBufferObject(_floorVertexDataObject.get());
    _floorVertexLayout->addComponent(AttributeType::Position, DataFormat::Vec3, sizeof(VertexData), quadBufferIdx,
                                     offsetof(VertexData, position));
    _floorVertexLayout->addComponent(AttributeType::TexCoord, DataFormat::Vec2, sizeof(VertexData), quadBufferIdx,
                                     offsetof(VertexData, tex_coord));
    _floorVertexLayout->addComponent(AttributeType::Normal, DataFormat::Vec3, sizeof(VertexData), quadBufferIdx,
                                     offsetof(VertexData, normal));
    _floorVertexLayout->finalize();

    _floorTexture = util::load_texture(renderer, "resources/wood.png");
    DrawCallCreateProperties props;
    props.vertexLayout = _floorVertexLayout.get();
    props.primitive_type = PrimitiveType::TriangleStrip;
    props.offset = 0;
    props.count = quadData.size();
    props.index_type = IndexType::None;
    _floorDrawCall = _renderer->getDrawCallManager()->createDrawCall(props);

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

//    auto light = _renderer->getLightingManager()->addLight(LightType::Point, false);
//    light->setPosition(glm::vec3(-3.f, 1.f, 0.f));
//    light->setColor(glm::vec3(2.4f, 8.2f, 5.3f));

    int width, height;
    SDL_GL_GetDrawableSize(SDL_GL_GetCurrentWindow(), &width, &height);

    _viewUniforms.projection_matrix = glm::perspectiveFov(45.0f, (float) width, (float) height, 0.01f, 50000.0f);

    _wholeFrameCategory = _renderer->getProfiler()->createCategory("Whole frame");
    nvgCreateFont(_renderer->getNanovgContext(), "sans", "resources/Roboto-Regular.ttf");

    _hdrRenderTarget = createHDRRenderTarget(width, height);

    _bloomRenderTargets[0] = createHDRRenderTarget(width, height);
    _bloomRenderTargets[1] = createHDRRenderTarget(width, height);

    PipelineProperties brightPassProps;
    brightPassProps.shaderType = ShaderType::HdrBrightpass;

    _brightPassState = _renderer->createPipelineState(brightPassProps);

    _fullscreenTriBuffer = _renderer->createBuffer(BufferType::Vertex);
    auto triData = getFullscreenTriData();
    _fullscreenTriBuffer->setData(triData.data(), triData.size() * sizeof(VertexData), BufferUsage::Static);

    _fullscreenTriLayout = _renderer->createVertexLayout();
    auto bufferIdx = _fullscreenTriLayout->attachBufferObject(_fullscreenTriBuffer.get());
    _fullscreenTriLayout->addComponent(AttributeType::Position,
                                       DataFormat::Vec3,
                                       sizeof(VertexData),
                                       bufferIdx,
                                       offsetof(VertexData, position));
    _fullscreenTriLayout->addComponent(AttributeType::TexCoord,
                                       DataFormat::Vec2,
                                       sizeof(VertexData),
                                       bufferIdx,
                                       offsetof(VertexData, tex_coord));

    _fullscreenTriLayout->finalize();

    DrawCallCreateProperties draw_call_properties;
    draw_call_properties.vertexLayout = _fullscreenTriLayout.get();
    draw_call_properties.primitive_type = PrimitiveType::Triangle;
    draw_call_properties.offset = 0;
    draw_call_properties.count = 3;
    draw_call_properties.index_type = IndexType::None;
    _fullscreenTriDrawCall = _renderer->getDrawCallManager()->createDrawCall(draw_call_properties);

    PipelineProperties hdrProps;
    hdrProps.shaderType = ShaderType::HdrPostProcessing;

    _hdrPipelineState = _renderer->createPipelineState(hdrProps);

    PipelineProperties bloomProps;
    bloomProps.shaderType = ShaderType::HdrBloom;

    _bloomPassState = _renderer->createPipelineState(bloomProps);


    _viewUniformBuffer = _renderer->createBuffer(BufferType::Uniform);
    _viewUniformBuffer->setData(nullptr, sizeof(ViewUniformData), BufferUsage::Streaming);

    _viewDescriptorSet = _renderer->createDescriptorSet(DescriptorSetType::ViewSet);
    auto descriptor = _viewDescriptorSet->getDescriptor(DescriptorSetPart::ViewSet_Uniforms);
    descriptor->setUniformBuffer(_viewUniformBuffer.get(), 0, sizeof(ViewUniformData));
}

Application::~Application() {
}

void Application::render(Renderer* renderer) {
    float radius = 3.0f;
    float camX = sin(_timing->getTotalTime()) * radius;
    float camZ = cos(_timing->getTotalTime()) * radius;

    _wholeFrameCategory->begin();

//    _renderer->getRenderTargetManager()->useRenderTarget(_hdrRenderTarget.get());
    renderer->clear(glm::vec4(0.f, 0.f, 0.f, 1.f));

//    auto matrices = _sunLight->beginShadowPass();
//    ViewUniformData shadowView;
//    shadowView.projection_matrix = matrices.projection;
//    shadowView.view_matrix = matrices.view;
//    shadowView.view_projection_matrix = shadowView.projection_matrix * shadowView.view_matrix;
//    _viewUniformBuffer->updateData(&shadowView, 0, sizeof(shadowView), UpdateFlags::DiscardOldData);
//
//    _viewDescriptorSet->bind();
//    renderScene();
//    _viewDescriptorSet->unbind();
//
//    _sunLight->endShadowPass();

    _viewUniforms.view_matrix =
        glm::lookAt(glm::vec3(camX, 3.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    _viewUniforms.view_projection_matrix = _viewUniforms.projection_matrix * _viewUniforms.view_matrix;
    _viewUniformBuffer->updateData(&_viewUniforms, 0, sizeof(_viewUniforms), UpdateFlags::DiscardOldData);

    _viewDescriptorSet->bind();
    _lightingManager.beginLightPass();

    renderScene();

    _lightingManager.endLightPass();

//    auto bloomed_texture = doBloomPass();

//    _renderer->getRenderTargetManager()->useRenderTarget(nullptr);
//
//    _hdrPipelineState->bind();
//    _fullscreenTriDrawCall->getParameters()->setFloat(ShaderParameterType::HdrExposure, 1.f);
//    _fullscreenTriDrawCall->getParameters()->setTexture(ShaderParameterType::ColorTexture,
//                                                        _hdrRenderTarget->getColorTexture());
//    _fullscreenTriDrawCall->getParameters()->setTexture(ShaderParameterType::BloomedTexture, bloomed_texture);
//    _fullscreenTriDrawCall->draw();

//    updateParticles();
//    _particleQuadDrawCall->drawInstanced(_particles.size());
    _viewDescriptorSet->unbind();

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

std::unique_ptr<RenderTarget> Application::createHDRRenderTarget(uint32_t width, uint32_t height) {
    RenderTargetProperties props;
    props.width = width;
    props.height = height;
    props.color_buffers = { ColorBufferFormat::RGB16F };
    props.depth.enable = false;
    props.depth.make_texture_handle = true;

    return _renderer->getRenderTargetManager()->createRenderTarget(props);
}

void Application::changeResolution(uint32_t width, uint32_t height) {
    auto settings = _renderer->getSettingsManager()->getCurrentSettings();
    settings.resolution = glm::uvec2(width, height);
    _renderer->getSettingsManager()->changeSettings(settings);

    _viewUniforms.projection_matrix = glm::perspectiveFov(45.0f, (float) width, (float) height, 0.01f, 50000.0f);

    _hdrRenderTarget = createHDRRenderTarget(width, height);
}

void Application::renderUI() {
    auto settings = _renderer->getSettingsManager()->getCurrentSettings();

    auto ctx = _renderer->getNanovgContext();

    nvgBeginFrame(ctx, settings.resolution.x, settings.resolution.y, 1.f);

    auto w = 300;
    auto h = 50;
    auto x = settings.resolution.x - w - 20;
    auto y = 20;

    drawTimes(ctx, _cpuTimes, 120, x, y, w, h, "CPU Time");

    y += h + 20;
    drawTimes(ctx, _gpuTimes, 120, x, y, w, h, "GPU Time");

    _renderer->nanovgEndFrame();
}
void Application::renderScene() {
    _model->prepareData(mat4());

    _model->render();

    _floorModelDescriptorSet->bind();
    _floorDrawCall->draw();
    _floorModelDescriptorSet->unbind();
}

TextureHandle* Application::doBloomPass() {
//    _renderer->getRenderTargetManager()->useRenderTarget(_bloomRenderTargets[0].get());
//    _brightPassState->bind();
//
//    _fullscreenTriDrawCall->getParameters()->setTexture(ShaderParameterType::ColorTexture,
//                                                        _hdrRenderTarget->getColorTexture());
//    _fullscreenTriDrawCall->draw();
//
//    // bloom target 0 now contains the parts of the scene that are bright
//    bool first_iter = true;
//    bool horizontal = true;
//
//    _bloomPassState->bind();
//    auto amount = 10;
//    for (auto i = 0; i < amount; ++i) {
//        auto index = horizontal ? 1 : 0;
//        auto other_index = horizontal ? 0 : 1;
//
//        _renderer->getRenderTargetManager()->useRenderTarget(_bloomRenderTargets[index].get());
//        _fullscreenTriDrawCall->getParameters()->setBoolean(ShaderParameterType::BloomHorizontal, horizontal);
//        _fullscreenTriDrawCall->getParameters()->setTexture(ShaderParameterType::ColorTexture,
//                                                            _bloomRenderTargets[other_index]->getColorTexture());
//        _fullscreenTriDrawCall->draw();
//
//        horizontal = !horizontal;
//        if (first_iter) {
//            first_iter = false;
//        }
//    }
//
//
//    return _bloomRenderTargets[horizontal ? 1 : 0]->getColorTexture();
    return nullptr;
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
