#pragma once

#include "PipelineState.hpp"
#include "ShaderParameters.hpp"
#include "Util.hpp"

enum class ClearTarget {
    Color = 1 << 0,
    Depth = 1 << 1,
    Stencil = 1 << 2,
};
template<>
struct BitOperationsTag<ClearTarget> {
    static constexpr bool value = true;
};

class CommandBuffer {
 public:
    virtual ~CommandBuffer() {}

    virtual void clear(const glm::vec4& color, ClearTarget target) = 0;

    virtual void bindPipeline(PointerWrapper<PipelineState> pipeline) = 0;

    virtual void bindVertexArrayObject(PointerWrapper<VertexArrayObject> vao) = 0;

    virtual void bindDescriptorSet(PointerWrapper<DescriptorSet> set) = 0;

    virtual void pushConstants(void* data, size_t size) = 0;

    virtual void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexOffset, uint32_t baseInstance) = 0;

    virtual void drawIndexed(uint32_t indexCount,
                             uint32_t instanceCount,
                             uint32_t indexOffset,
                             uint32_t baseVertex,
                             uint32_t baseInstance) = 0;
};
