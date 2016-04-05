//
//

#include "textures.hpp"

#define STB_IMAGE_IMPLEMENTATION

#include "util/stb_image.h"

std::unique_ptr<Texture2D> util::load_texture(Renderer *renderer, const std::string &path) {

    auto texture = renderer->createTexture();
    int width, height, components;
    auto texture_data = stbi_load(path.c_str(), &width, &height, &components, 0);
    if (texture_data) {
        auto stride = width * components;
        std::unique_ptr<uint8_t[]> buffer(new uint8_t[stride]);
        size_t height_half = width / 2;
        for (size_t y = 0; y < height_half; ++y) {
            uint8_t *top = texture_data + y * stride;
            uint8_t *bottom = texture_data + (height - y - 1) * stride;

            memcpy(buffer.get(), top, stride);
            memcpy(top, bottom, stride);
            memcpy(bottom, buffer.get(), stride);
        }

        texture->initialize(width, height, components == 3 ? TextureFormat::R8G8B8 : TextureFormat::R8G8B8A8,
                            texture_data);

        stbi_image_free(texture_data);
    }

    return texture;
}

