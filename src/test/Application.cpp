
#include "Application.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <vector>
#include <cmath>

using namespace glm;

const double rotationPerSecond = M_PI_4 / 2;

Application::Application() : xAngle(0.f), yAngle(0.f) {
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

    _projMx = glm::perspectiveFov(45.0f, (float)width, (float)height, 0.01f, 50000.0f);
}

void Application::render(Renderer *renderer) {
    renderer->clear(glm::vec4(0.f, 0.f, 0.f, 1.f));

    _model->drawModel(renderer, _projMx, _viewMx, _modelMx);

    renderer->presentNextFrame();
}

void Application::deinitialize(Renderer *renderer) {
    _model.reset();
}

void Application::handleEvent(SDL_Event* event) {
    switch(event->type) {
        case SDL_MOUSEMOTION:
            if (!(event->motion.state & SDL_BUTTON_LMASK)) {
                break;
            }
            auto xrel = event->motion.xrel;
            auto yrel = event->motion.yrel;

            xAngle += xrel * rotationPerSecond * _timing->getFrametime();
            yAngle += yrel * rotationPerSecond * _timing->getFrametime();
            updateModelMatrix();
            break;
    }
}

void Application::updateModelMatrix() {
    _modelMx = glm::eulerAngleYZ(xAngle, yAngle);
}
