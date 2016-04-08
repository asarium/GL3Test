//
//

#include "GL3Util.hpp"
#include "GL3Renderer.hpp"

#include <glm/glm.hpp>

namespace {
    struct VertexData {
        glm::vec3 position;
        glm::vec2 tex_coord;
    };

    std::vector<VertexData> getFullscreenTriData() {
        std::vector<VertexData> data;
        data.push_back({glm::vec3(-1.0f, -1.0f, 0.f),
                        glm::vec2(0.f, 0.f)
                       });
        data.push_back({glm::vec3(3.0f, -1.0f, 0.f),
                        glm::vec2(2.f, 0.f)
                       });
        data.push_back({glm::vec3(-1.0f, 3.0f, 0.f),
                        glm::vec2(0.f, 2.f)
                       });

        return data;
    }

    std::vector<VertexData> getQuadData() {
        std::vector<VertexData> data;
        data.push_back({glm::vec3(-1.0f, -1.0f, 0.f),
                        glm::vec2(0.f, 0.f)
                       });
        data.push_back({glm::vec3(-1.0f, 1.0f, 0.f),
                        glm::vec2(0.f, 1.f)
                       });
        data.push_back({glm::vec3(1.0f, -1.0f, 0.f),
                        glm::vec2(1.f, 0.f)
                       });
        data.push_back({glm::vec3(1.0f, 1.0f, 0.f),
                        glm::vec2(1.f, 1.f)
                       });

        return data;
    }
}

GL3Util::GL3Util(GL3Renderer *renderer) : GL3Object(renderer) {
    {
        _fullscreenTriBuffer = _renderer->createBuffer(BufferType::Vertex);
        auto quadData = getFullscreenTriData();
        _fullscreenTriBuffer->setData(quadData.data(), quadData.size() * sizeof(VertexData), BufferUsage::Static);

        auto layout = _renderer->createVertexLayout();
        auto vertexBufferIndex = layout->attachBufferObject(_fullscreenTriBuffer.get());
        layout->addComponent(AttributeType::Position, DataFormat::Vec3, sizeof(VertexData), vertexBufferIndex,
                             offsetof(VertexData, position));
        layout->addComponent(AttributeType::TexCoord, DataFormat::Vec2, sizeof(VertexData), vertexBufferIndex,
                             offsetof(VertexData, tex_coord));
        layout->finalize();

        _fullscreenTriLayout.reset(static_cast<GL3VertexLayout *>(layout.release()));
    }
    {
        _quadBuffer = _renderer->createBuffer(BufferType::Vertex);
        auto quadData = getQuadData();
        _quadBuffer->setData(quadData.data(), quadData.size() * sizeof(VertexData), BufferUsage::Static);

        auto layout = _renderer->createVertexLayout();
        auto vertexBufferIndex = layout->attachBufferObject(_quadBuffer.get());
        layout->addComponent(AttributeType::Position, DataFormat::Vec3, sizeof(VertexData), vertexBufferIndex,
                             offsetof(VertexData, position));
        layout->addComponent(AttributeType::TexCoord, DataFormat::Vec2, sizeof(VertexData), vertexBufferIndex,
                             offsetof(VertexData, tex_coord));
        layout->finalize();

        _quadLayout.reset(static_cast<GL3VertexLayout *>(layout.release()));
    }
}

void GL3Util::drawFullScreenTri() {
    _fullscreenTriLayout->bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void GL3Util::drawQuad() {
    _quadLayout->bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}




