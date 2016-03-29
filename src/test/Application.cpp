
#include "Application.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

using namespace glm;

Application::Application(){
}

Application::~Application() {
}

void Application::initialize(Renderer *renderer, Timing* time) {
    _timing = time;

    _model.reset(new AssimpModel());
    _model->loadModel(renderer, "resources/duck.dae");

    int width, height;
    SDL_GL_GetDrawableSize(renderer->getWindow(), &width, &height);

    _viewMx = glm::translate(mat4(), -glm::vec3(0.0f, 0.0f, 180.0f));
    _modelMx = mat4();

    _projMx = glm::perspectiveFov(45.0f, (float)width, (float)height, 0.01f, 50000.0f);
}

void Application::render(Renderer *renderer) {
    float radius = 5.0f;
    float camX = sin(_timing->getTotalTime()) * radius;
    float camZ = cos(_timing->getTotalTime()) * radius;
    _viewMx = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

    renderer->clear(glm::vec4(0.f, 0.f, 0.f, 1.f));

    _model->drawModel(renderer, _projMx, _viewMx, _modelMx);

    renderer->presentNextFrame();
}

void Application::deinitialize(Renderer *renderer) {
    _model.reset();
}

void Application::handleEvent(SDL_Event*) {
}
