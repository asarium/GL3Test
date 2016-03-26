#include <vector>
#include "Application.hpp"

#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

Application::Application() {
}

Application::~Application() {
}

void Application::initialize(Renderer *renderer) {
    _model.reset(new AssimpModel());
    _model->loadModel(renderer, "resources/teapots.DAE");

    int width, height;
    SDL_GL_GetDrawableSize(renderer->getWindow(), &width, &height);

    _viewMx = glm::translate(mat4(), -glm::vec3(0.0f, 0.5f, 60.0f));

    _projMx = glm::perspective(160.0f, width / (float) height, 0.01f, 50000.0f);
}

void Application::render(Renderer *renderer, Timing *) {
    renderer->clear(glm::vec4(0.f, 0.f, 0.f, 1.f));

    _model->drawModel(renderer, _projMx, _viewMx, mat4());

    renderer->presentNextFrame();
}

void Application::deinitialize(Renderer *renderer) {
    _model.reset();
}


