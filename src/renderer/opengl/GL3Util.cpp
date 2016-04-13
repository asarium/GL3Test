//
//

#include "GL3Util.hpp"
#include "GL3Renderer.hpp"

#include <glm/glm.hpp>
#include <iostream>

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

    std::pair<std::vector<glm::vec3>, std::vector<uint16_t>> generate_sphere_mesh(size_t rings, size_t segments) {
        // Code copied from the original deferred rendering backend of FSO
        auto nVertex = (rings + 1) * (segments+1);
        auto nIndex = 6 * rings * (segments + 1);

        std::vector<glm::vec3> vertices;
        vertices.reserve(nVertex);
        std::vector<uint16_t> indices;
        indices.reserve(nIndex);

        auto fDeltaRingAngle = (M_PI / rings);
        auto fDeltaSegAngle = (2.0f * M_PI / segments);
        uint16_t wVerticeIndex = 0 ;

        // Generate the group of rings for the sphere
        for( size_t ring = 0; ring <= rings; ring++ ) {
            auto r0 = sin(ring * fDeltaRingAngle);
            auto y0 = cos(ring * fDeltaRingAngle);

            // Generate the group of segments for the current ring
            for(size_t seg = 0; seg <= segments; seg++) {
                auto x0 = r0 * sin(seg * fDeltaSegAngle);
                auto z0 = r0 * cos(seg * fDeltaSegAngle);

                // Add one vertex to the strip which makes up the sphere
                vertices.push_back(glm::vec3(x0, y0, z0));

                if (ring != rings) {
                    // each vertex (except the last) has six indices pointing to it
                    indices.push_back(wVerticeIndex + (uint16_t)segments + 1);
                    indices.push_back(wVerticeIndex);
                    indices.push_back(wVerticeIndex + (uint16_t)segments);
                    indices.push_back(wVerticeIndex + (uint16_t)segments + 1);
                    indices.push_back(wVerticeIndex + 1);
                    indices.push_back(wVerticeIndex);
                    wVerticeIndex ++;
                }
            }; // end for seg
        } // end for ring

        return std::make_pair(std::move(vertices), std::move(indices));
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
    {
        auto dataPair = generate_sphere_mesh(16, 16);

        _sphereVertexData = _renderer->createBuffer(BufferType::Vertex);
        _sphereVertexData->setData(dataPair.first.data(), sizeof(glm::vec3) * dataPair.first.size(), BufferUsage::Static);

        _sphereIndexData = _renderer->createBuffer(BufferType::Index);
        _sphereIndexData->setData(dataPair.second.data(), sizeof(uint16_t) * dataPair.second.size(), BufferUsage::Static);

        auto layout = _renderer->createVertexLayout();
        auto vertexIdx = layout->attachBufferObject(_sphereVertexData.get());
        auto indexIdx = layout->attachBufferObject(_sphereIndexData.get());

        layout->addComponent(AttributeType::Position, DataFormat::Vec3, sizeof(glm::vec3), vertexIdx, 0);
        layout->setIndexBuffer(indexIdx);

        layout->finalize();

        _numSphereIndices = (GLsizei) dataPair.second.size();
        _sphereLayout.reset(static_cast<GL3VertexLayout *>(layout.release()));
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

void GL3Util::drawSphere() {
    _sphereLayout->bind();
    glDrawElements(GL_TRIANGLES, _numSphereIndices, GL_UNSIGNED_SHORT, nullptr);
}


bool checkFrameBufferStatus() {
    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (status) {
        case GL_FRAMEBUFFER_COMPLETE:
            return true;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            std::cout << "FBO error: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"
                << std::endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            std::cout << "FBO error: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"
                << std::endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            std::cout << "FBO error: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"
                << std::endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            std::cout << "FBO error: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"
                << std::endl;
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            std::cout << "FBO error: GL_FRAMEBUFFER_UNSUPPORTED"
                << std::endl;
            break;
        default:
            std::cout << "FBO error: Unknown error"
                << std::endl;
            break;
    }

    return false;
}

