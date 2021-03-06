//
//

#include "GL3CommandBuffer.hpp"
#include <glad/glad.h>
#include "GL3State.hpp"
#include "GL3PipelineState.hpp"
#include "GL3VertexLayout.hpp"
#include "GL3Renderer.hpp"
#include "GL3ShaderParameters.hpp"


GL3CommandBuffer::GL3CommandBuffer(GL3Renderer* renderer) : GL3Object(renderer), _currentPrimitiveType(GL_NONE) {
}

void GL3CommandBuffer::clear(const glm::vec4& color, ClearTarget target) {
    if (false) {
        glDebugMessageInsertARB(GL_DEBUG_SOURCE_APPLICATION_ARB,
                                GL_DEBUG_TYPE_OTHER_ARB,
                                0,
                                GL_DEBUG_SEVERITY_LOW_ARB,
                                -1,
                                "Clearing screen");
    }

    GLenum mask = 0;
    if (target & ClearTarget::Color) {
        mask |= GL_COLOR_BUFFER_BIT;
        glClearColor(color.r, color.g, color.b, color.a);
    }
    if (target & ClearTarget::Depth) {
        mask |= GL_DEPTH_BUFFER_BIT;
        GLState->setDepthMask(true);
    }
    if (target & ClearTarget::Stencil) {
        mask |= GL_STENCIL_BUFFER_BIT;
    }

    glClear(mask);
}

void GL3CommandBuffer::bindPipeline(PointerWrapper<PipelineState> pipeline) {
    if (false) {
        glDebugMessageInsertARB(GL_DEBUG_SOURCE_APPLICATION_ARB,
                                GL_DEBUG_TYPE_OTHER_ARB,
                                0,
                                GL_DEBUG_SEVERITY_LOW_ARB,
                                -1,
                                "Binding pipeline");
    }

    auto glState = static_cast<GL3PipelineState*>(&pipeline);

    glState->setupState();
    _currentPrimitiveType = glState->getPrimitiveType();
}

void GL3CommandBuffer::bindVertexArrayObject(PointerWrapper<VertexArrayObject> vao) {
    if (false) {
        glDebugMessageInsertARB(GL_DEBUG_SOURCE_APPLICATION_ARB,
                                GL_DEBUG_TYPE_OTHER_ARB,
                                0,
                                GL_DEBUG_SEVERITY_LOW_ARB,
                                -1,
                                "Binding VAO");
    }

    auto glVao = static_cast<GL3VertexArrayObject*>(&vao);

    glVao->bind();

    _vao = glVao;
}

void GL3CommandBuffer::pushConstants(void* data, size_t size) {
    if (false) {
        glDebugMessageInsertARB(GL_DEBUG_SOURCE_APPLICATION_ARB,
                                GL_DEBUG_TYPE_OTHER_ARB,
                                0,
                                GL_DEBUG_SEVERITY_LOW_ARB,
                                -1,
                                "Setting push constants");
    }
    _renderer->getPushConstantManager()->setConstants(data, size);
}

void
GL3CommandBuffer::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexOffset, uint32_t baseInstance) {
    Assertion(baseInstance == 0, "Base instance it not implemented!");
    Assertion(_vao, "A Vertex Array Object has to be set for drawing!");

    glDrawArraysInstanced(_currentPrimitiveType, vertexOffset, vertexCount, instanceCount);
}

void GL3CommandBuffer::drawIndexed(uint32_t indexCount,
                                   uint32_t instanceCount,
                                   uint32_t indexOffset,
                                   uint32_t baseVertex,
                                   uint32_t baseInstance) {
    Assertion(baseInstance == 0, "Base instance it not implemented!");
    Assertion(_vao, "A Vertex Array Object has to be set for drawing!");
    Assertion(_vao->getIndexType() != GL_NONE, "Vertex Array Object needs index information for indexed rendering!");

    auto indices = _vao->computeIndexOffset(indexOffset);

    glDrawElementsInstancedBaseVertex(_currentPrimitiveType,
                                      indexCount,
                                      _vao->getIndexType(),
                                      indices,
                                      instanceCount,
                                      baseVertex);
}
void GL3CommandBuffer::bindDescriptorSet(PointerWrapper<DescriptorSet> set) {
    if (false) {
        glDebugMessageInsertARB(GL_DEBUG_SOURCE_APPLICATION_ARB,
                                GL_DEBUG_TYPE_OTHER_ARB,
                                0,
                                GL_DEBUG_SEVERITY_LOW_ARB,
                                -1,
                                "Binding descriptor set");
    }

    auto glSet = static_cast<GL3DescriptorSet*>(&set);

    glSet->bind();
}
void GL3CommandBuffer::unbindDescriptorSet(PointerWrapper<DescriptorSet> set) {
    if (false) {
        glDebugMessageInsertARB(GL_DEBUG_SOURCE_APPLICATION_ARB,
                                GL_DEBUG_TYPE_OTHER_ARB,
                                0,
                                GL_DEBUG_SEVERITY_LOW_ARB,
                                -1,
                                "Unbinding descriptor set");
    }

    auto glSet = static_cast<GL3DescriptorSet*>(&set);

    glSet->unbind();
}
