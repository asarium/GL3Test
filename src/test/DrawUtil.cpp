//
//

#include "DrawUtil.hpp"

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

std::pair<std::vector<glm::vec3>, std::vector<uint16_t>> generate_sphere_mesh(size_t rings, size_t segments) {
    // Code copied from the original deferred rendering backend of FSO
    auto nVertex = (rings + 1) * (segments + 1);
    auto nIndex = 6 * rings * (segments + 1);

    std::vector<glm::vec3> vertices;
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
            vertices.push_back(glm::vec3(x0, y0, z0));

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

DrawUtil::DrawUtil(Renderer* renderer) {
    {
        _fullscreenTriBuffer = renderer->createBuffer(BufferType::Vertex);
        auto quadData = getFullscreenTriData();
        _fullscreenTriBuffer->setData(quadData.data(), quadData.size() * sizeof(VertexData), BufferUsage::Static);

        _fullscreenTriLayout = renderer->createVertexLayout();
        auto vertexBufferIndex = _fullscreenTriLayout->attachBufferObject(_fullscreenTriBuffer.get());
        _fullscreenTriLayout->addComponent(AttributeType::Position,
                                           DataFormat::Vec3,
                                           sizeof(VertexData),
                                           vertexBufferIndex,
                                           offsetof(VertexData, position));
        _fullscreenTriLayout->addComponent(AttributeType::TexCoord,
                                           DataFormat::Vec2,
                                           sizeof(VertexData),
                                           vertexBufferIndex,
                                           offsetof(VertexData, tex_coord));
        _fullscreenTriLayout->finalize();

        DrawCallCreateProperties props;
        props.primitive_type = PrimitiveType::Triangle;
        props.index_type = IndexType::None;
        props.count = 3;

        props.vertexLayout = _fullscreenTriLayout.get();

        _fullscreenTriDrawCall = renderer->getDrawCallManager()->createDrawCall(props);
    }
    {
        _quadBuffer = renderer->createBuffer(BufferType::Vertex);
        auto quadData = getQuadData();
        _quadBuffer->setData(quadData.data(), quadData.size() * sizeof(VertexData), BufferUsage::Static);

        _quadLayout = renderer->createVertexLayout();
        auto vertexBufferIndex = _quadLayout->attachBufferObject(_quadBuffer.get());
        _quadLayout->addComponent(AttributeType::Position, DataFormat::Vec3, sizeof(VertexData), vertexBufferIndex,
                                  offsetof(VertexData, position));
        _quadLayout->addComponent(AttributeType::TexCoord, DataFormat::Vec2, sizeof(VertexData), vertexBufferIndex,
                                  offsetof(VertexData, tex_coord));
        _quadLayout->finalize();

        DrawCallCreateProperties props;
        props.primitive_type = PrimitiveType::TriangleStrip;
        props.index_type = IndexType::None;
        props.count = 4;

        props.vertexLayout = _quadLayout.get();

        _quadDrawCall = renderer->getDrawCallManager()->createDrawCall(props);
    }
    {
        auto dataPair = generate_sphere_mesh(16, 16);

        _sphereVertexData = renderer->createBuffer(BufferType::Vertex);
        _sphereVertexData->setData(dataPair.first.data(),
                                   sizeof(glm::vec3) * dataPair.first.size(),
                                   BufferUsage::Static);

        _sphereIndexData = renderer->createBuffer(BufferType::Index);
        _sphereIndexData->setData(dataPair.second.data(),
                                  sizeof(uint16_t) * dataPair.second.size(),
                                  BufferUsage::Static);

        _sphereLayout = renderer->createVertexLayout();
        auto vertexIdx = _sphereLayout->attachBufferObject(_sphereVertexData.get());
        auto indexIdx = _sphereLayout->attachBufferObject(_sphereIndexData.get());

        _sphereLayout->addComponent(AttributeType::Position, DataFormat::Vec3, sizeof(glm::vec3), vertexIdx, 0);
        _sphereLayout->setIndexBuffer(indexIdx);

        _sphereLayout->finalize();

        DrawCallCreateProperties props;
        props.primitive_type = PrimitiveType::Triangle;
        props.count = dataPair.second.size();
        props.index_type = IndexType::Short;
        props.vertexLayout = _sphereLayout.get();

        _quadDrawCall = renderer->getDrawCallManager()->createDrawCall(props);
    }
}
void DrawUtil::drawSphere() {
    _sphereDrawCall->draw();
}
void DrawUtil::drawFullscreenTri() {
    _fullscreenTriDrawCall->draw();
}
