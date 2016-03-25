//
//

#include <SDL.h>
#include "Timing.hpp"

Timing::Timing() {
    SDL_InitSubSystem(SDL_INIT_TIMER);

    _baseTime = SDL_GetPerformanceCounter();
    _frequency = SDL_GetPerformanceFrequency();

    _lastTime = _baseTime;
    _currentTime = _baseTime;
}

Timing::~Timing() {
    SDL_QuitSubSystem(SDL_INIT_TIMER);
}

void Timing::tick() {
    _lastTime = _currentTime;
    _currentTime = SDL_GetPerformanceCounter();
}

float Timing::getFrametime() {
    return (_currentTime - _lastTime) / (float)_frequency;
}

float Timing::getTotalTime() {
    return (_currentTime - _baseTime) / (float)_frequency;
}
