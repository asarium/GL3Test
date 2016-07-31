#pragma once

#include "renderer/CommandBuffer.hpp"
#include "GL3Object.hpp"
#include <glad/glad.h>
#include "GL3VertexLayout.hpp"


class GL3CommandBuffer final: public CommandBuffer, GL3Object
{
    GLenum _currentPrimitiveType;
    GL3VertexArrayObject* _vao;
public:
    explicit GL3CommandBuffer(GL3Renderer* renderer);
    ~GL3CommandBuffer() {}

    void clear(const glm::vec4& color, ClearTarget target) override;

    void bindPipeline(PointerWrapper<PipelineState> pipeline) override;

    void bindVertexArrayObject(PointerWrapper<VertexArrayObject> vao) override;

    void bindDescriptorSet(PointerWrapper<DescriptorSet> set) override;

    void pushConstants(void* data, size_t size) override;
    
    void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexOffset, uint32_t baseInstance) override;
    
    void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t indexOffset, uint32_t baseVertex, uint32_t baseInstance) override;
};
