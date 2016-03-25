#pragma once

#include <SDL_timer.h>

class Timing {
    Uint64 _baseTime;
    Uint64 _frequency;

    Uint64 _lastTime;
    Uint64 _currentTime;
public:
    Timing();
    ~Timing();

    void tick();

    float getFrametime();
    float getTotalTime();
};


