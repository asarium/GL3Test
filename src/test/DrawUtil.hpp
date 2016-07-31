#pragma once

#include <renderer/BufferObject.hpp>

#include <memory>
#include <renderer/VertexLayout.hpp>
#include <renderer/Renderer.hpp>

struct MeshProperties
{
    bool indexed;
    uint32_t count;
};

class DrawMesh {
    std::unique_ptr<VertexArrayObject> _vao;
    
    bool _indexed;
    uint32_t _count;

public:
    DrawMesh(std::unique_ptr<VertexArrayObject>&& vertex_array_object, bool indexed, uint32_t count);

    void draw(CommandBuffer* cmd, uint32_t instances = 1);
};

class DrawUtil {
    Renderer* _renderer;

    std::unique_ptr<BufferObject> _fullscreenTriBuffer;
    MeshProperties _fullscreenProps;

    std::unique_ptr<BufferObject> _sphereVertexData;
    std::unique_ptr<BufferObject> _sphereIndexData;
    uint32_t _sphereNumIndices;
    MeshProperties _sphereProps;

    VertexInputStateProperties _vertexInputProps;
 public:
    explicit DrawUtil(Renderer* renderer);

    void setVertexProperties(PipelineProperties& props) const;

    std::unique_ptr<DrawMesh> getFullscreenTriMesh() const;

    std::unique_ptr<DrawMesh> getSphereMesh() const;
};


