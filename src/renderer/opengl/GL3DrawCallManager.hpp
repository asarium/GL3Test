#pragma once

#include "renderer/DrawCallManager.hpp"
#include "GL3VertexLayout.hpp"
#include "GL3ShaderManager.hpp"
#include "GL3PipelineState.hpp"
#include "GL3Object.hpp"

struct GL3DrawCallProperties {
    GL3VertexLayout* vertexLayout;

    GLenum primitive_type;
    GLsizei count;
    GLint offset;

    bool instanced;

    bool indexed;
    struct {
        GLenum type;
    } index;

    GLint base_vertex;
    GLuint range_begin;
    GLuint range_end;

    inline bool hasBaseVertex() {
        return base_vertex != 0;
    }

    inline bool hasRange() {
        return range_begin != range_end;
    }
};

class GL3DrawCallManager final: public GL3Object, public DrawCallManager {
    GL3ShaderManager* _manager;

    GL3DrawCallProperties convertProperties(const DrawCallCreateProperties& props);

 public:
    GL3DrawCallManager(GL3Renderer* renderer, GL3ShaderManager* _manager) : GL3Object(renderer), _manager(_manager) { }

    virtual ~GL3DrawCallManager() { };

    virtual std::unique_ptr<DrawCall> createDrawCall(const DrawCallCreateProperties& props) override;

    virtual std::unique_ptr<VariableDrawCall> createVariableDrawCall(const DrawCallCreateProperties& props) override;


    virtual std::unique_ptr<InstancedDrawCall> createInstancedDrawCall(const DrawCallCreateProperties& props) override;

    virtual std::unique_ptr<InstancedVariableDrawCall>
        createInstancedVariableDrawCall(const DrawCallCreateProperties& props) override;
};


