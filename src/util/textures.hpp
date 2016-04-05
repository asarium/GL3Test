#pragma once

#include "renderer/Renderer.hpp"
#include "renderer/Texture2D.hpp"

#include <memory>

namespace util {
    std::unique_ptr<Texture2D> load_texture(Renderer* renderer, const std::string& path);
}
