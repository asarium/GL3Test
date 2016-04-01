//
//

#include "GL3LightingManager.hpp"

GL3LightingManager::GL3LightingManager(GL3Renderer * renderer) : _renderer(renderer)
{
}

Light *GL3LightingManager::addLight(LightType type) {
    _lights.emplace_back(new GL3Light(type));
    return _lights.back().get();
}

void GL3LightingManager::removeLight(Light *light) {

}

void GL3LightingManager::clearLights() {
    _lights.clear();
}

void GL3LightingManager::beginLightPass() {

}

void GL3LightingManager::endLightPass() {

}


GL3Light::GL3Light(LightType in_type) : type(in_type) {

}

void GL3Light::setPosition(const glm::vec3 &pos) {
    position = pos;
}

void GL3Light::setDirection(const glm::vec3 &dir) {
    direction = dir;
}

void GL3Light::setColor(const glm::vec3 &color) {
    this->color = color;
}







