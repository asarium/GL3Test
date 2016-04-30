#pragma once

#include "renderer/DrawCallManager.hpp"
#include "GL3VertexLayout.hpp"
#include "GL3ShaderProgram.hpp"
#include "GL3ShaderManager.hpp"
#include "GL3PipelineState.hpp"

struct GL3DrawCallProperties {
    GL3VertexLayout *vertexLayout;

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

    GL3DrawCallProperties convertProperties(const DrawCallCreateProperties &props);

public:
    GL3DrawCallManager(GL3ShaderManager *_manager) : _manager(_manager) { }

    virtual ~GL3DrawCallManager() { };

    virtual std::unique_ptr<DrawCall> createDrawCall(const DrawCallCreateProperties& props) override;

    virtual std::unique_ptr<VariableDrawCall> createVariableDrawCall(const DrawCallCreateProperties& props) override;


    virtual std::unique_ptr<InstancedDrawCall> createInstancedDrawCall(const DrawCallCreateProperties& props) override;

    virtual std::unique_ptr<InstancedVariableDrawCall>
        createInstancedVariableDrawCall(const DrawCallCreateProperties& props) override;
};


