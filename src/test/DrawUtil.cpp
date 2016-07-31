//
//

#include "DrawUtil.hpp"

#include <math.h>

namespace {

struct VertexData {
    glm::vec3 position;
    glm::vec2 tex_coord;
};

std::vector<VertexData> getFullscreenTriData() {
    std::vector<VertexData> data;
    data.push_back({
                       glm::vec3(-1.0f, -1.0f, 0.f),
                       glm::vec2(0.f, 0.f)
                   });
    data.push_back({
                       glm::vec3(3.0f, -1.0f, 0.f),
                       glm::vec2(2.f, 0.f)
                   });
    data.push_back({
                       glm::vec3(-1.0f, 3.0f, 0.f),
                       glm::vec2(0.f, 2.f)
                   });

    return data;
}

std::vector<VertexData> getQuadData() {
    std::vector<VertexData> data;
    data.push_back({
                       glm::vec3(-1.0f, -1.0f, 0.f),
                       glm::vec2(0.f, 0.f)
                   });
    data.push_back({
                       glm::vec3(-1.0f, 1.0f, 0.f),
                       glm::vec2(0.f, 1.f)
                   });
    data.push_back({
                       glm::vec3(1.0f, -1.0f, 0.f),
                       glm::vec2(1.f, 0.f)
                   });
    data.push_back({
                       glm::vec3(1.0f, 1.0f, 0.f),
                       glm::vec2(1.f, 1.f)
                   });

    return data;
}

std::pair<std::vector<VertexData>, std::vector<uint16_t>> generate_sphere_mesh(size_t rings, size_t segments) {
    // Code copied from the original deferred rendering backend of FSO
    auto nVertex = (rings + 1) * (segments + 1);
    auto nIndex = 6 * rings * (segments + 1);

    std::vector<VertexData> vertices;
    vertices.reserve(nVertex);
    std::vector<uint16_t> indices;
    indices.reserve(nIndex);

    auto fDeltaRingAngle = (M_PI / rings);
    auto fDeltaSegAngle = (2.0f * M_PI / segments);
    uint16_t wVerticeIndex = 0;

    // Generate the group of rings for the sphere
    for (size_t ring = 0; ring <= rings; ring++) {
        auto r0 = sin(ring * fDeltaRingAngle);
        auto y0 = cos(ring * fDeltaRingAngle);

        // Generate the group of segments for the current ring
        for (size_t seg = 0; seg <= segments; seg++) {
            auto x0 = r0 * sin(seg * fDeltaSegAngle);
            auto z0 = r0 * cos(seg * fDeltaSegAngle);

            // Add one vertex to the strip which makes up the sphere
            VertexData vertex;
            vertex.position = glm::vec3(x0, y0, z0);
            vertex.tex_coord = glm::vec2(0.f, 0.f);
            vertices.push_back(vertex);

            if (ring != rings) {
                // each vertex (except the last) has six indices pointing to it
                indices.push_back(wVerticeIndex + (uint16_t) segments + 1);
                indices.push_back(wVerticeIndex);
                indices.push_back(wVerticeIndex + (uint16_t) segments);
                indices.push_back(wVerticeIndex + (uint16_t) segments + 1);
                indices.push_back(wVerticeIndex + 1);
                indices.push_back(wVerticeIndex);
                wVerticeIndex++;
            }
        }; // end for seg
    } // end for ring

    return std::make_pair(std::move(vertices), std::move(indices));
}
}

DrawMesh::DrawMesh(std::unique_ptr<VertexArrayObject>&& vertex_array_object, bool indexed, uint32_t count): _vao(std::move(vertex_array_object)),
                                                                                                            _indexed(indexed),
                                                                                                            _count(count)
{
}

void DrawMesh::draw(CommandBuffer* cmd, uint32_t instances)
{
    cmd->bindVertexArrayObject(_vao.get());

    if (_indexed)
    {
        cmd->drawIndexed(_count, instances, 0, 0, 0);
    }
    else
    {
        cmd->draw(_count, instances, 0, 0);
    }
}

DrawUtil::DrawUtil(Renderer* renderer) {
    _renderer = renderer;
    {
        _vertexInputProps.addComponent(AttributeType::Position, 0, DataFormat::Vec3, offsetof(VertexData, position));
        _vertexInputProps.addComponent(AttributeType::TexCoord, 0, DataFormat::Vec2, offsetof(VertexData, tex_coord));

        _vertexInputProps.addBufferBinding(0, false, sizeof(VertexData));
    }
    {
        _fullscreenTriBuffer = _renderer->createBuffer(BufferType::Vertex);
        auto quadData = getFullscreenTriData();
        _fullscreenTriBuffer->setData(quadData.data(), quadData.size() * sizeof(VertexData), BufferUsage::Static);
    }
    {
        auto dataPair = generate_sphere_mesh(16, 16);

        _sphereVertexData = _renderer->createBuffer(BufferType::Vertex);
        _sphereVertexData->setData(dataPair.first.data(),
                                   sizeof(VertexData) * dataPair.first.size(),
                                   BufferUsage::Static);

        _sphereIndexData = _renderer->createBuffer(BufferType::Index);
        _sphereIndexData->setData(dataPair.second.data(),
                                  sizeof(uint16_t) * dataPair.second.size(),
                                  BufferUsage::Static);
        
        _sphereNumIndices = (uint32_t) dataPair.second.size();
    }
}

void DrawUtil::setVertexProperties(PipelineProperties& props) const
{
    props.vertexInput = _vertexInputProps;
    props.primitive_type = PrimitiveType::Triangle;
}

std::unique_ptr<DrawMesh> DrawUtil::getFullscreenTriMesh() const
{
    VertexArrayProperties props;
    props.addBufferBinding(0, _fullscreenTriBuffer.get());

    auto vao = _renderer->createVertexArrayObject(_vertexInputProps, props);

    return std::unique_ptr<DrawMesh>(new DrawMesh(std::move(vao), false, 3));
}

std::unique_ptr<DrawMesh> DrawUtil::getSphereMesh() const
{
    VertexArrayProperties props;
    props.addBufferBinding(0, _sphereVertexData.get());

    props.indexBuffer = _sphereIndexData.get();
    props.indexOffset = 0;
    props.indexType = IndexType::Short;

    auto vao = _renderer->createVertexArrayObject(_vertexInputProps, props);

    return std::unique_ptr<DrawMesh>(new DrawMesh(std::move(vao), false, 3));
}
