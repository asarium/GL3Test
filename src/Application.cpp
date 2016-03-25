#include <vector>
#include "Application.hpp"

#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

struct VertexData {
    glm::vec4 position;
};

Application::Application() {
}

Application::~Application() {
}

void Application::initialize(Renderer *renderer) {
    std::vector<VertexData> data;
    std::vector<int> indices;

    data.push_back({vec4(-0.15f, 0.0f, 0.0f, 1.0f)});
    data.push_back({vec4(0.15f, 0.0f, 0.0f, 1.0f)});
    data.push_back({vec4(0.15f, 0.25f, 0.0f, 1.0f)});
    data.push_back({vec4(-0.15f, 0.25f, 0.0f, 1.0f)});

    data.push_back({vec4(-0.5f, 0.25f, 0.0f, 1.0f)});
    data.push_back({vec4(0.5f, 0.25f, 0.0f, 1.0f)});
    data.push_back({vec4(0.0f, 1.5f, 0.0f, 1.0f)});

    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(1);

    indices.push_back(2);
    indices.push_back(0);
    indices.push_back(3);

    indices.push_back(6);
    indices.push_back(4);
    indices.push_back(5);

    _vertex_buffer = renderer->createBuffer(BufferType::Vertex);
    _vertex_buffer->setData(data.data(), data.size() * sizeof(data[0]), BufferUsage::Static);

    _index_buffer = renderer->createBuffer(BufferType::Index);
    _index_buffer->setData(indices.data(), indices.size() * sizeof(indices[0]), BufferUsage::Static);

    _vertex_layout = renderer->createVertexLayout();
    auto vertex_idx = _vertex_layout->attachBufferObject(_vertex_buffer.get());
    auto index_idx = _vertex_layout->attachBufferObject(_index_buffer.get());

    _vertex_layout->addComponent(AttributeType::Position, DataFormat::Vec4, sizeof(VertexData), vertex_idx,
                                 offsetof(VertexData, position));
    _vertex_layout->setIndexBuffer(index_idx);

    _vertex_layout->finalize();

    auto window = renderer->getWindow();
    int width, height;
    SDL_GL_GetDrawableSize(window, &width, &height);

    _shader = renderer->createShader(ShaderType::Model);
    auto projMX = glm::perspective(45.0f, width/(float)height, 0.01f,100.0f);
    _shader->setParameterMat4(ShaderParameter::ProjectionMatrix, projMX);

    auto viewMx = glm::translate(mat4(), -glm::vec3(0.0f, 0.5f, 3.0f));
    _shader->setParameterMat4(ShaderParameter::ViewMatrix, viewMx);
    _shader->setParameterMat4(ShaderParameter::ModelMatrix, glm::mat4());

    DrawCallProperties props;
    props.shader = _shader.get();
    props.vertexLayout = _vertex_layout.get();
    props.state.depth_test = true;

    _drawCall = renderer->createDrawCall(props);
}

void Application::render(Renderer *renderer, Timing *) {
    renderer->clear(glm::vec4(0.f, 0.f, 0.f, 1.f));

    _drawCall->drawIndexed(PrimitiveType::Triangle, 9, IndexType::Integer);

    renderer->presentNextFrame();
}

void Application::deinitialize(Renderer *renderer) {
    _vertex_layout.reset();

    _vertex_buffer.reset();
    _index_buffer.reset();

    _shader.reset();
    _drawCall.reset();
}


