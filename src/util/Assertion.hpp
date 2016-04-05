#pragma once

#include <SDL_assert.h>

#define Assertion(condition, message) SDL_assert((condition) && (message))
