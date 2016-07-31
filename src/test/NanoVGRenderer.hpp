#pragma once

#include <renderer/nanovg/nanovg.h>
#include <renderer/Renderer.hpp>

#include <unordered_map>
#include <util/UniformAligner.hpp>
#include <util/VariableUniformBuffer.hpp>

NVGcontext* createNanoVGContext(Renderer* renderer);

void deleteNanoVGContext(NVGcontext* context);

class NanoVGRenderer
{
    enum class CallType {
        Fill,
        ConvexFill,
        Stroke,
        Triangles
    };
    enum class ShaderType : int32_t {
        FillGradient = 0,
        FillImage = 1,
        Simple = 2,
        Image = 3
    };
    struct Image {
        std::unique_ptr<Texture> tex;
        int type;
        int flags;
    };
    struct DrawCall {
        CallType type;

        uint32_t triangleOffset;
        uint32_t triangleCount;

        uint32_t pathOffset;
        uint32_t pathCount;

        size_t uniformIndex;

        int image;
    };
    struct Path
    {
        uint32_t fillOffset;
        uint32_t fillCount;
        uint32_t strokeOffset;
        uint32_t strokeCount;
    };
    struct UniformData {
        float scissorMat[12]; // matrices are actually 3 vec4s
        float paintMat[12];
        NVGcolor innerCol;
        NVGcolor outerCol;
        float scissorExt[2];
        float scissorScale[2];
        float extent[2];
        float radius;
        float feather;
        float strokeMult;
        float strokeThr;
        int texType;
        ShaderType type;
    };
    struct GlobalUniformData {
        glm::vec2 viewSize;
    };

    Renderer* _renderer;

    std::unique_ptr<BufferObject> _vertexBuffer;
    std::unique_ptr<VertexArrayObject> _vertexArrayObject;

    std::unique_ptr<VariableUniformBuffer> _uniformBuffer;
    UniformAligner<UniformData, sizeof(GlobalUniformData)> _uniformAligner;

    std::unique_ptr<DescriptorSet> _globalDescriptorSet;


    std::unique_ptr<PipelineState> _trianglesPipelineState;
    std::unique_ptr<PipelineState> _triangleFillPipelineState;
    std::unique_ptr<PipelineState> _triangleStrokePipelineState;

    std::unique_ptr<PipelineState> _fillShapePipelineState;
    std::unique_ptr<PipelineState> _fillAntiAliasPipelineState;
    std::unique_ptr<PipelineState> _fillFillPipelineState;

    std::unique_ptr<PipelineState> _strokeFillPipelineState;
    std::unique_ptr<PipelineState> _strokeAntiaiasPipelineState;
    std::unique_ptr<PipelineState> _strokeClearStencilPipelineState;

    std::vector<NVGvertex> _vertices;
    std::vector<DrawCall> _drawCalls;
    std::vector<Path> _paths;

    std::unordered_map<int, Image> _textureMap;
    int _lastImageId;

    glm::ivec2 _viewport;

    DrawCall* addDrawCall();
    size_t addUniformData(size_t num);
    size_t addVertices(const NVGvertex* vert, size_t num);

    size_t addVertices(size_t num);
    size_t addPaths(size_t num);

    bool convertPaint(UniformData* frag, NVGpaint* paint, NVGscissor* scissor, float width, float fringe, float strokeThr);

    Image* getTexture(int id);

    std::unique_ptr<PipelineState> createPipelineState(const PipelineProperties& props,
                                                       const VertexInputStateProperties& vertexProps,
                                                       PrimitiveType primitive);

    std::unique_ptr<DescriptorSet> createAndBindUniforms(CommandBuffer* cmd, size_t uniform_index, int image);

    void drawTriangles(CommandBuffer* cmd, const DrawCall& call);
    void drawFill(CommandBuffer* cmd, const DrawCall& call);
    void drawConvexFill(CommandBuffer* cmd, const DrawCall& call);
    void drawStroke(CommandBuffer* cmd, const DrawCall& call);
public:
    explicit NanoVGRenderer(Renderer* renderer);

    void initialize();

    void setViewport(int width, int height);

    void renderFill(NVGpaint* paint, NVGscissor* scissor, float fringe, const float* bounds, const NVGpath* paths, int npaths);

    void renderStroke(NVGpaint* paint, NVGscissor* scissor, float fringe, float strokeWidth, const NVGpath* paths, int npaths);

    void renderTriangles(NVGpaint* paint, NVGscissor* scissor, const NVGvertex* verts, int nverts);

    void renderFlush();

    void renderCancel();

    int createTexture(int type, int w, int h, int imageFlags, const unsigned char* data);

    int deleteTexture(int image);

    int updateTexture(int image, int x, int y, int w, int h, const unsigned char* data);

    int getTextureSize(int image, int* w, int* h);
};
