#pragma once

#include "renderer/DrawCallManager.hpp"
#include "GL3VertexLayout.hpp"
#include "GL3ShaderProgram.hpp"
#include "GL3ShaderManager.hpp"
#include "GL3PipelineState.hpp"

struct GL3DrawCallProperties {
    GL3VertexLayout *vertexLayout;
    GL3PipelineState *state;

    GLenum primitive_type;
    GLsizei count;
    GLint offset;

    bool instanced;

    bool indexed;
    struct {
        GLenum type;
    } index;
};

class GL3DrawCallManager : public DrawCallManager {
    GL3ShaderManager *_manager;

    GL3DrawCallProperties convertProperties(const DrawCallProperties &props);

public:
    GL3DrawCallManager(GL3ShaderManager *_manager) : _manager(_manager) { }

    virtual ~GL3DrawCallManager() { };

    virtual std::unique_ptr<DrawCall> createDrawCall(const DrawCallProperties &props, PrimitiveType type,
                                                     size_t offset, size_t count) override;

    virtual std::unique_ptr<DrawCall> createIndexedCall(const DrawCallProperties &props, PrimitiveType type,
                                                        size_t offset, size_t count, IndexType indexType) override;

    virtual std::unique_ptr<VariableDrawCall> createVariableDrawCall(const DrawCallProperties &props,
                                                                     PrimitiveType type) override;

    virtual std::unique_ptr<VariableDrawCall> createVariableIndexedCall(const DrawCallProperties &props,
                                                                        PrimitiveType type,
                                                                        IndexType indexType) override;


    virtual std::unique_ptr<InstancedDrawCall> createInstancedDrawCall(const DrawCallProperties &props, PrimitiveType type,
                                                              size_t offset, size_t count) override;

    virtual std::unique_ptr<InstancedDrawCall> createInstancedIndexedCall(const DrawCallProperties &props, PrimitiveType type,
                                                                 size_t offset, size_t count,
                                                                 IndexType indexType) override;

    virtual std::unique_ptr<InstancedVariableDrawCall> createInstancedVariableDrawCall(const DrawCallProperties &props,
                                                                                       PrimitiveType type) override;

    virtual std::unique_ptr<InstancedVariableDrawCall> createInstancedVariableIndexedCall(const DrawCallProperties &props,
                                                                                 PrimitiveType type,
                                                                                 IndexType indexType) override;
};


