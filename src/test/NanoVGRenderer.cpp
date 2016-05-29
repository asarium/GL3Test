#include "NanoVGRenderer.hpp"
#include <util/Assertion.hpp>
#include <gli/texture2d.hpp>
#include <gli/generate_mipmaps.hpp>

namespace {
int nvgRenderCreate(void* userptr) {
    auto renderer = static_cast<NanoVGRenderer*>(userptr);

    try {
        renderer->initialize();
        return 1;
    } catch (...) {
        return 0;
    }
}

void nvgRenderDelete(void* userptr) {
    auto renderer = static_cast<NanoVGRenderer*>(userptr);
    delete renderer;
}

void nvgViewport(void* userptr, int width, int height) {
    auto renderer = static_cast<NanoVGRenderer*>(userptr);
    renderer->setViewport(width, height);
}

void nvgRenderTriangles(void* userptr, NVGpaint* paint, NVGscissor* scissor, const NVGvertex* verts, int nverts) {
    auto renderer = static_cast<NanoVGRenderer*>(userptr);
    renderer->renderTriangles(paint, scissor, verts, nverts);
}
void nvgRenderFill(void* userptr,
                   NVGpaint* paint,
                   NVGscissor* scissor,
                   float fringe,
                   const float* bounds,
                   const NVGpath* paths,
                   int npaths) {
    auto renderer = static_cast<NanoVGRenderer*>(userptr);
    renderer->renderFill(paint, scissor, fringe, bounds, paths, npaths);
}
void nvgRenderStroke(void* userptr,
                     NVGpaint* paint,
                     NVGscissor* scissor,
                     float fringe,
                     float strokeWidth,
                     const NVGpath* paths,
                     int npaths) {
    auto renderer = static_cast<NanoVGRenderer*>(userptr);
    renderer->renderStroke(paint, scissor, fringe, strokeWidth, paths, npaths);
}
void nvgRenderFlush(void* userptr) {
    auto renderer = static_cast<NanoVGRenderer*>(userptr);
    renderer->renderFlush();

}
void nvgRenderCancel(void* userptr) {
    auto renderer = static_cast<NanoVGRenderer*>(userptr);
    renderer->renderCancel();
}
int nvgCreateTexture(void* userptr, int type, int w, int h, int imageFlags, const unsigned char* data) {
    auto renderer = static_cast<NanoVGRenderer*>(userptr);
    return renderer->createTexture(type, w, h, imageFlags, data);
}
int nvgUpdateTexture(void* userptr, int image, int x, int y, int w, int h, const unsigned char* data) {
    auto renderer = static_cast<NanoVGRenderer*>(userptr);
    return renderer->updateTexture(image, x, y, w, h, data);
}
int nvgDeleteTexture(void* userptr, int image) {
    auto renderer = static_cast<NanoVGRenderer*>(userptr);
    return renderer->deleteTexture(image);
}
int nvgGetTextureSize(void* userptr, int image, int* w, int* h) {
    auto renderer = static_cast<NanoVGRenderer*>(userptr);
    return renderer->getTextureSize(image, w, h);
}

NVGcolor premulColor(NVGcolor c) {
    c.r *= c.a;
    c.g *= c.a;
    c.b *= c.a;
    return c;
}
void xformToMat3x4(float* m3, float* t) {
    m3[0] = t[0];
    m3[1] = t[1];
    m3[2] = 0.0f;
    m3[3] = 0.0f;
    m3[4] = t[2];
    m3[5] = t[3];
    m3[6] = 0.0f;
    m3[7] = 0.0f;
    m3[8] = t[4];
    m3[9] = t[5];
    m3[10] = 1.0f;
    m3[11] = 0.0f;
}

size_t maxVertCount(const NVGpath* paths, size_t npaths) {
    size_t count = 0;
    for (size_t i = 0; i < npaths; i++) {
        count += paths[i].nfill;
        count += paths[i].nstroke;
    }
    return count;
}

void vertexSet(NVGvertex* vtx, float x, float y, float u, float v) {
    vtx->x = x;
    vtx->y = y;
    vtx->u = u;
    vtx->v = v;
}

PipelineProperties getDefaultPipelineProperties() {
    PipelineProperties props;
    props.shaderType = ShaderType::NanoVGShader;
    props.shaderFlags = ShaderFlags::NanoVGEdgeAA;

    props.enableBlending = true;
    props.blendFunction = BlendFunction::PremultAlpha;

    props.enableFaceCulling = true;
    props.culledFace = CullFace::Back;
    props.frontFace = FrontFace::CounterClockWise;

    props.depthMode = DepthMode::None;

    props.enableScissor = false;

    props.colorMask = glm::bvec4(true, true, true, true);

    props.stencilMask = 0xffffffff;
    props.stencilFunc = std::make_tuple(ComparisionFunction::Always, 0, 0xffffffff);
    props.setStencilOp(std::make_tuple(StencilOperation::Keep, StencilOperation::Keep, StencilOperation::Keep));

    return props;
}
}

NVGcontext* createNanoVGContext(Renderer* renderer) {
    Assertion(renderer != nullptr, "Invalid renderer passed!");

    NVGparams params;
    NVGcontext* ctx = nullptr;
    auto nvgRenderer = new NanoVGRenderer(renderer);

    memset(&params, 0, sizeof(params));
    params.renderCreate = nvgRenderCreate;
    params.renderDelete = nvgRenderDelete;
    params.renderViewport = nvgViewport;

    params.renderTriangles = nvgRenderTriangles;
    params.renderFill = nvgRenderFill;
    params.renderStroke = nvgRenderStroke;
    params.renderFlush = nvgRenderFlush;
    params.renderCancel = nvgRenderCancel;

    params.renderCreateTexture = nvgCreateTexture;
    params.renderUpdateTexture = nvgUpdateTexture;
    params.renderDeleteTexture = nvgDeleteTexture;
    params.renderGetTextureSize = nvgGetTextureSize;

    params.userPtr = nvgRenderer;
    params.edgeAntiAlias = 1;

    ctx = nvgCreateInternal(&params);
    if (ctx == nullptr) {
        return nullptr;
    }

    return ctx;
}

void deleteNanoVGContext(NVGcontext* context) {
    Assertion(context != nullptr, "Invalid context passed!");

    nvgDeleteInternal(context);
}

std::unique_ptr<VariableDrawCall> NanoVGRenderer::createDrawCall(PrimitiveType type) const {
    DrawCallCreateProperties drawProps;
    drawProps.primitive_type = type;
    drawProps.vertexLayout = _vertexLayout.get();

    return _renderer->getDrawCallManager()->createVariableDrawCall(drawProps);
}

NanoVGRenderer::NanoVGRenderer(Renderer* renderer)
    : _renderer(renderer), _uniformAligner(renderer->getLimits().uniform_offset_alignment), _lastImageId(0) {
}

void NanoVGRenderer::initialize() {
    _vertexBuffer = _renderer->createBuffer(BufferType::Vertex);

    _vertexLayout = _renderer->createVertexLayout();
    auto bufferIdx = _vertexLayout->attachBufferObject(_vertexBuffer.get());

    _vertexLayout->addComponent(AttributeType::Position2D,
                                DataFormat::Vec2,
                                sizeof(NVGvertex),
                                bufferIdx,
                                offsetof(NVGvertex, x));
    _vertexLayout->addComponent(AttributeType::TexCoord,
                                DataFormat::Vec2,
                                sizeof(NVGvertex),
                                bufferIdx,
                                offsetof(NVGvertex, u));
    _vertexLayout->finalize();

    _triangleDrawCall = createDrawCall(PrimitiveType::Triangle);
    _triangleFanDrawCall = createDrawCall(PrimitiveType::TriangleFan);
    _triangleStripDrawCall = createDrawCall(PrimitiveType::TriangleStrip);

    _uniformBuffer = VariableUniformBuffer::createVariableBuffer(_renderer);

    _globalDescriptorSet = _renderer->createDescriptorSet(DescriptorSetType::NanoVGGlobalSet);
    _globalDescriptorSet->getDescriptor(DescriptorSetPart::NanoVGGlobalSet_Uniforms)->setUniformBuffer(_uniformBuffer->buffer(),
                                                                                                       0,
                                                                                                       sizeof(GlobalUniformData));

    // Initialize the global data
    _uniformAligner.getHeader<GlobalUniformData>()->viewSize = glm::vec2(0.f, 0.f);

    // Now create all the pipeline states that we are going to need
    {
        // Pipeline state for simple triangle draws
        _trianglesPipelineState = _renderer->createPipelineState(getDefaultPipelineProperties());
    }
    {
        // Pipeline states for the fill shader
        auto props = getDefaultPipelineProperties();
        props.enableStencil = true;
        props.stencilMask = 0xFF;
        props.stencilFunc = std::make_tuple(ComparisionFunction::Always, 0, 0xFF);
        props.colorMask = glm::bvec4(false, false, false, false);

        props.frontStencilOp =
            std::make_tuple(StencilOperation::Keep, StencilOperation::Keep, StencilOperation::IncrementWrap);
        props.backStencilOp =
            std::make_tuple(StencilOperation::Keep, StencilOperation::Keep, StencilOperation::DecrementWrap);

        props.enableFaceCulling = false;
        _fillShapePipelineState = _renderer->createPipelineState(props);

        props.enableFaceCulling = true;
        props.colorMask = glm::bvec4(true, true, true, true);
        props.stencilFunc = std::make_tuple(ComparisionFunction::Equal, 0x00, 0xFF);
        props.setStencilOp(std::make_tuple(StencilOperation::Keep, StencilOperation::Keep, StencilOperation::Keep));
        _fillAntiAliasPipelineState = _renderer->createPipelineState(props);

        props.stencilFunc = std::make_tuple(ComparisionFunction::NotEqual, 0x00, 0xFF);
        props.setStencilOp(std::make_tuple(StencilOperation::Zero, StencilOperation::Zero, StencilOperation::Zero));
        _fillFillPipelineState = _renderer->createPipelineState(props);
    }
    {
        // Pipeline states for the stroke shader
        auto props = getDefaultPipelineProperties();
        props.enableStencil = true;
        props.stencilMask = 0xFF;
        props.stencilFunc = std::make_tuple(ComparisionFunction::Equal, 0x00, 0xFF);
        props.setStencilOp(std::make_tuple(StencilOperation::Keep, StencilOperation::Keep, StencilOperation::Increment));
        _strokeFillPipelineState = _renderer->createPipelineState(props);

        props.setStencilOp(std::make_tuple(StencilOperation::Keep, StencilOperation::Keep, StencilOperation::Keep));
        _strokeAntiaiasPipelineState = _renderer->createPipelineState(props);

        props.colorMask = glm::bvec4(false, false, false, false);
        props.stencilFunc = std::make_tuple(ComparisionFunction::Always, 0x00, 0xFF);
        props.setStencilOp(std::make_tuple(StencilOperation::Zero, StencilOperation::Zero, StencilOperation::Zero));
        _strokeClearStencilPipelineState = _renderer->createPipelineState(props);
    }
}

void NanoVGRenderer::setViewport(int width, int height) {
    _viewport = glm::ivec2(width, height);
}

void NanoVGRenderer::renderFill(NVGpaint* paint,
                                NVGscissor* scissor,
                                float fringe,
                                const float* bounds,
                                const NVGpath* paths,
                                int npaths) {
    auto call = addDrawCall();

    call->type = CallType::Fill;
    call->pathOffset = addPaths((size_t) npaths);
    call->pathCount = (size_t) npaths;
    call->image = paint->image;

    if (npaths == 1 && paths[0].convex) {
        call->type = CallType::ConvexFill;
    }

    // Allocate vertices for all the paths.
    auto maxverts = maxVertCount(paths, (size_t) npaths) + 6;
    auto offset = addVertices(maxverts);

    for (int i = 0; i < npaths; i++) {
        Path* copy = &_paths[call->pathOffset + i];
        const NVGpath* path = &paths[i];
        memset(copy, 0, sizeof(Path));
        if (path->nfill > 0) {
            copy->fillOffset = offset;
            copy->fillCount = (size_t) path->nfill;
            memcpy(&_vertices[offset], path->fill, sizeof(NVGvertex) * path->nfill);
            offset += path->nfill;
        }
        if (path->nstroke > 0) {
            copy->strokeOffset = offset;
            copy->strokeCount = (size_t) path->nstroke;
            memcpy(&_vertices[offset], path->stroke, sizeof(NVGvertex) * path->nstroke);
            offset += path->nstroke;
        }
    }

    // Quad
    call->triangleOffset = offset;
    call->triangleCount = 6;
    auto quad = &_vertices[call->triangleOffset];
    vertexSet(&quad[0], bounds[0], bounds[3], 0.5f, 1.0f);
    vertexSet(&quad[1], bounds[2], bounds[3], 0.5f, 1.0f);
    vertexSet(&quad[2], bounds[2], bounds[1], 0.5f, 1.0f);

    vertexSet(&quad[3], bounds[0], bounds[3], 0.5f, 1.0f);
    vertexSet(&quad[4], bounds[2], bounds[1], 0.5f, 1.0f);
    vertexSet(&quad[5], bounds[0], bounds[1], 0.5f, 1.0f);

    // Setup uniforms for draw calls
    if (call->type == CallType::Fill) {
        call->uniformIndex = addUniformData(2);
        // Simple shader for stencil
        auto frag = _uniformAligner.getElement(call->uniformIndex);
        memset(frag, 0, sizeof(*frag));
        frag->strokeThr = -1.0f;
        frag->type = ShaderType::Simple;
        // Fill shader
        convertPaint(_uniformAligner.getElement(call->uniformIndex + 1), paint, scissor, fringe, fringe, -1.0f);
    }
    else {
        call->uniformIndex = addUniformData(1);
        // Fill shader
        convertPaint(_uniformAligner.getElement(call->uniformIndex), paint, scissor, fringe, fringe, -1.0f);
    }
}
void NanoVGRenderer::renderTriangles(NVGpaint* paint, NVGscissor* scissor, const NVGvertex* verts, int nverts) {
    _vertices.insert(_vertices.end(), verts, verts + nverts);

    auto call = addDrawCall();
    call->type = CallType::Triangles;
    call->triangleCount = static_cast<size_t>(nverts);
    call->triangleOffset = addVertices(verts, static_cast<size_t>(nverts));
    call->image = paint->image;

    call->uniformIndex = addUniformData(1);

    auto uniformData = _uniformAligner.getElement(call->uniformIndex);
    auto succcess = convertPaint(uniformData, paint, scissor, 1.0f, 1.0f, -1.0f);

    Assertion(succcess, "Failed to convert paint, probably caused by an invalid texture handle.");

    uniformData->type = ShaderType::Image;
}
void NanoVGRenderer::renderStroke(NVGpaint* paint,
                                  NVGscissor* scissor,
                                  float fringe,
                                  float strokeWidth,
                                  const NVGpath* paths,
                                  int npaths) {
    auto call = addDrawCall();

    call->type = CallType::Stroke;
    call->pathOffset = addPaths((size_t) npaths);
    call->pathCount = (size_t) npaths;
    call->image = paint->image;

    // Allocate vertices for all the paths.
    auto maxverts = maxVertCount(paths, (size_t) npaths);
    auto offset = addVertices(maxverts);

    for (int i = 0; i < npaths; i++) {
        Path* copy = &_paths[call->pathOffset + i];
        const NVGpath* path = &paths[i];
        memset(copy, 0, sizeof(*copy));
        if (path->nstroke) {
            copy->strokeOffset = offset;
            copy->strokeCount = (size_t) path->nstroke;
            memcpy(&_vertices[offset], path->stroke, sizeof(NVGvertex) * path->nstroke);
            offset += path->nstroke;
        }
    }
    // Fill shader
    call->uniformIndex = addUniformData(2);

    convertPaint(_uniformAligner.getElement(call->uniformIndex), paint, scissor, strokeWidth, fringe, -1.0f);
    convertPaint(_uniformAligner.getElement(call->uniformIndex + 1),
                 paint,
                 scissor,
                 strokeWidth,
                 fringe,
                 1.0f - 0.5f / 255.0f);
}
void NanoVGRenderer::renderFlush() {
    if (_drawCalls.empty()) {
        return;
    }

    // First update changed data
    _uniformAligner.getHeader<GlobalUniformData>()->viewSize = _viewport;

    _uniformBuffer->setData(_uniformAligner.getData(), _uniformAligner.getSize());
    _vertexBuffer->setData(_vertices.data(), sizeof(NVGvertex) * _vertices.size(), BufferUsage::Streaming);

    _globalDescriptorSet->bind();

    for (auto& drawCall : _drawCalls) {
        switch (drawCall.type) {
            case CallType::Fill:
                drawFill(drawCall);
                break;
            case CallType::ConvexFill:
                drawConvexFill(drawCall);
                break;
            case CallType::Stroke:
                drawStroke(drawCall);
                break;
            case CallType::Triangles:
                drawTriangles(drawCall);
                break;
        }
    }

    // Reset all data again
    _vertices.clear();
    _uniformAligner.resize(0);
    _drawCalls.clear();
    _paths.clear();
}
void NanoVGRenderer::renderCancel() {
    // Clear all data written by the render functions
    _vertices.clear();
    _uniformAligner.resize(0);
    _drawCalls.clear();
    _paths.clear();
}
int NanoVGRenderer::createTexture(int type, int w, int h, int imageFlags, const unsigned char* data) {
    gli::format format;
    if (type == NVG_TEXTURE_RGBA) {
        format = gli::FORMAT_RGBA8_SNORM_PACK8;
    } else {
        format = gli::FORMAT_A8_UNORM_PACK8;
    }

    FilterProperties filterProps;
    if (imageFlags & NVG_IMAGE_REPEATX) {
        filterProps.wrap_behavior_s = WrapBehavior::Repeat;
    } else {
        filterProps.wrap_behavior_s = WrapBehavior::ClampToEdge;
    }
    if (imageFlags & NVG_IMAGE_REPEATY) {
        filterProps.wrap_behavior_t = WrapBehavior::Repeat;
    } else {
        filterProps.wrap_behavior_t = WrapBehavior::ClampToEdge;
    }

    filterProps.magnification_filter = FilterMode::Linear;
    if (imageFlags & NVG_IMAGE_GENERATE_MIPMAPS) {
        filterProps.minification_filter = FilterMode::LinearMipmapLinear;
    } else {
        filterProps.minification_filter = FilterMode::Linear;
    }

    auto renderTexture = _renderer->createTexture();
    if (data != nullptr) {
        gli::texture2d texture(format, gli::extent2d(w, h));
        gli::texture2d* textureRef = &texture;
        std::memcpy(texture.data(), data, texture.size());

        // GLI doesn't like assigning the return value to generate mipmaps in any case but only use it if the flag is set
        gli::texture2d mipmapped = gli::generate_mipmaps(texture, gli::FILTER_LINEAR);
        if (imageFlags & NVG_IMAGE_GENERATE_MIPMAPS) {
            textureRef = &mipmapped;
        }
        renderTexture->initialize(*textureRef, filterProps);
    } else {
        // Only allocate storage
        AllocationProperties props;
        props.filterProperties = filterProps;
        props.format = format;
        props.size = gli::extent3d(w, h, 0);
        props.target = gli::TARGET_2D;

        renderTexture->allocate(props);
    }

    Image img;
    img.tex = std::move(renderTexture);
    img.type = type;
    img.flags = imageFlags;

    auto id = ++_lastImageId;
    _textureMap.insert(std::make_pair(id, std::move(img)));

    return id;
}
int NanoVGRenderer::updateTexture(int image, int x, int y, int w, int h, const unsigned char* data) {
    auto iter = _textureMap.find(image);
    if (iter == _textureMap.end()) {
        return 0;
    }

    auto& texture = iter->second;

    gli::extent3d pos(0, 0, 0);
    gli::format format;
    if (texture.type == NVG_TEXTURE_RGBA) {
        format = gli::FORMAT_RGBA8_SNORM_PACK8;
    } else {
        format = gli::FORMAT_A8_UNORM_PACK8;
    }

    // TODO: This could probably be done better by only uploading the changed are
    texture.tex->update(pos, texture.tex->getSize(), format, data);

    return 1;
}
int NanoVGRenderer::deleteTexture(int image) {
    auto iter = _textureMap.find(image);
    if (iter == _textureMap.end()) {
        return 0;
    }

    // This will call the destructor of the class and free the resources
    _textureMap.erase(iter);

    return 1;
}
int NanoVGRenderer::getTextureSize(int image, int* w, int* h) {
    auto img = getTexture(image);
    if (img == nullptr) {
        return 0;
    }

    auto size = img->tex->getSize();
    *w = size.x;
    *h = size.y;

    return 1;
}
NanoVGRenderer::DrawCall* NanoVGRenderer::addDrawCall() {
    _drawCalls.emplace_back();
    return &_drawCalls.back();
}

size_t NanoVGRenderer::addUniformData(size_t num) {
    auto current = _uniformAligner.getNumElements();
    _uniformAligner.resize(_uniformAligner.getNumElements() + num);
    return current;
}

size_t NanoVGRenderer::addVertices(const NVGvertex* vert, size_t num) {
    auto offset = _vertices.size();
    _vertices.insert(_vertices.end(), vert, vert + num);
    return offset;
}

size_t NanoVGRenderer::addVertices(size_t num) {
    auto offset = _vertices.size();
    _vertices.resize(offset + num);
    return offset;
}

size_t NanoVGRenderer::addPaths(size_t num) {
    auto offset = _paths.size();
    _paths.resize(offset + num);
    return offset;
}

bool NanoVGRenderer::convertPaint(UniformData* frag,
                                  NVGpaint* paint,
                                  NVGscissor* scissor,
                                  float width,
                                  float fringe,
                                  float strokeThr) {
    float invxform[6];

    memset(frag, 0, sizeof(*frag));

    frag->innerCol = premulColor(paint->innerColor);
    frag->outerCol = premulColor(paint->outerColor);

    if (scissor->extent[0] < -0.5f || scissor->extent[1] < -0.5f) {
        memset(frag->scissorMat, 0, sizeof(frag->scissorMat));
        frag->scissorExt[0] = 1.0f;
        frag->scissorExt[1] = 1.0f;
        frag->scissorScale[0] = 1.0f;
        frag->scissorScale[1] = 1.0f;
    }
    else {
        nvgTransformInverse(invxform, scissor->xform);
        xformToMat3x4(frag->scissorMat, invxform);

        frag->scissorExt[0] = scissor->extent[0];
        frag->scissorExt[1] = scissor->extent[1];
        frag->scissorScale[0] =
            sqrtf(scissor->xform[0] * scissor->xform[0] + scissor->xform[2] * scissor->xform[2]) / fringe;
        frag->scissorScale[1] =
            sqrtf(scissor->xform[1] * scissor->xform[1] + scissor->xform[3] * scissor->xform[3]) / fringe;
    }

    memcpy(frag->extent, paint->extent, sizeof(frag->extent));
    frag->strokeMult = (width * 0.5f + fringe * 0.5f) / fringe;
    frag->strokeThr = strokeThr;

    if (paint->image != 0) {
        auto tex = getTexture(paint->image);
        if (tex == nullptr) {
            return false;
        }
        if ((tex->flags & NVG_IMAGE_FLIPY) != 0) {
            float flipped[6];
            nvgTransformScale(flipped, 1.0f, -1.0f);
            nvgTransformMultiply(flipped, paint->xform);
            nvgTransformInverse(invxform, flipped);
        }
        else {
            nvgTransformInverse(invxform, paint->xform);
        }
        frag->type = ShaderType::FillImage;

        if (tex->type == NVG_TEXTURE_RGBA) {
            frag->texType = (tex->flags & NVG_IMAGE_PREMULTIPLIED) ? 0 : 1;
        } else {
            frag->texType = 2;
        }
        //		printf("frag->texType = %d\n", frag->texType);
    }
    else {
        frag->type = ShaderType::FillGradient;
        frag->radius = paint->radius;
        frag->feather = paint->feather;
        nvgTransformInverse(invxform, paint->xform);
    }

    xformToMat3x4(frag->paintMat, invxform);

    return true;
}

NanoVGRenderer::Image* NanoVGRenderer::getTexture(int id) {
    auto iter = _textureMap.find(id);
    if (iter == _textureMap.end()) {
        return nullptr;
    }
    return &iter->second;
}
std::unique_ptr<DescriptorSet> NanoVGRenderer::createAndBindUniforms(size_t uniform_index, int image) {
    auto tex = getTexture(image);
    auto descriptor = _renderer->createDescriptorSet(DescriptorSetType::NanoVGLocalSet);
    if (tex == nullptr) {
        descriptor->getDescriptor(DescriptorSetPart::NanoVGLocalSet_Texture)->setTexture(nullptr);
    } else {
        descriptor->getDescriptor(DescriptorSetPart::NanoVGLocalSet_Texture)->setTexture(tex->tex.get());
    }
    descriptor->getDescriptor(DescriptorSetPart::NanoVGLocalSet_Uniforms)->
        setUniformBuffer(_uniformBuffer->buffer(),
                         _uniformAligner.getOffset(uniform_index),
                         sizeof(UniformData));

    descriptor->bind();

    return descriptor;
}

void NanoVGRenderer::drawTriangles(const DrawCall& call) {
    auto descriptor = createAndBindUniforms(call.uniformIndex, call.image);

    _trianglesPipelineState->bind();

    _triangleDrawCall->draw(call.triangleOffset, call.triangleCount);
}
void NanoVGRenderer::drawFill(const DrawCall& call) {
    auto shapeDescriptor = createAndBindUniforms(call.uniformIndex, 0);

    _fillShapePipelineState->bind();
    auto pathOffset = call.pathOffset;
    for (size_t i = 0; i < call.pathCount; ++i) {
        _triangleFanDrawCall->draw(_paths[pathOffset + i].fillOffset, _paths[pathOffset + i].fillCount);
    }

    auto fillDescriptor = createAndBindUniforms(call.uniformIndex + 1, call.image);
    _fillAntiAliasPipelineState->bind();
    // Draw fringes
    for (size_t i = 0; i < call.pathCount; ++i) {
        _triangleStripDrawCall->draw(_paths[pathOffset + i].strokeOffset, _paths[pathOffset + i].strokeCount);
    }

    _fillFillPipelineState->bind();
    _triangleDrawCall->draw(call.triangleOffset, call.triangleCount);
}
void NanoVGRenderer::drawConvexFill(const DrawCall& call) {
    _trianglesPipelineState->bind();
    auto descriptor = createAndBindUniforms(call.uniformIndex, call.image);

    auto pathOffset = call.pathOffset;
    for (size_t i = 0; i < call.pathCount; ++i) {
        _triangleFanDrawCall->draw(_paths[pathOffset + i].fillOffset, _paths[pathOffset + i].fillCount);
    }
    // Draw fringes
    for (size_t i = 0; i < call.pathCount; ++i) {
        _triangleStripDrawCall->draw(_paths[pathOffset + i].strokeOffset, _paths[pathOffset + i].strokeCount);
    }
}
void NanoVGRenderer::drawStroke(const DrawCall& call) {
    auto pathOffset = call.pathOffset;

    // Fill the stroke base without overlap
    _strokeFillPipelineState->bind();
    auto fillDescriptor = createAndBindUniforms(call.uniformIndex + 1, call.image);
    for (size_t i = 0; i < call.pathCount; ++i) {
        _triangleStripDrawCall->draw(_paths[pathOffset + i].strokeOffset, _paths[pathOffset + i].strokeCount);
    }

    // Draw anti-aliased pixels.
    _strokeAntiaiasPipelineState->bind();
    auto aaDescriptor = createAndBindUniforms(call.uniformIndex, call.image);
    for (size_t i = 0; i < call.pathCount; ++i) {
        _triangleStripDrawCall->draw(_paths[pathOffset + i].strokeOffset, _paths[pathOffset + i].strokeCount);
    }

    // Clear stencil buffer.
    _strokeClearStencilPipelineState->bind();
    for (size_t i = 0; i < call.pathCount; ++i) {
        _triangleStripDrawCall->draw(_paths[pathOffset + i].strokeOffset, _paths[pathOffset + i].strokeCount);
    }
}


