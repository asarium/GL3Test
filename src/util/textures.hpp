#pragma once

#include "renderer/Renderer.hpp"
#include "renderer/Texture.hpp"

#include <memory>

namespace util {
    std::unique_ptr<Texture> load_texture(Renderer* renderer, const std::string& path);
}
