//
//

#include <gli/texture2d.hpp>
#include <gli/generate_mipmaps.hpp>
#include "textures.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC

#include "util/stb_image.h"

std::unique_ptr<Texture> util::load_texture(Renderer* renderer, const std::string& path) {

    auto render_texture = renderer->createTexture();
    int width, height, components;
    auto texture_data = stbi_load(path.c_str(), &width, &height, &components, 0);
    if (texture_data) {
        auto stride = width * components;
        std::unique_ptr<uint8_t[]> buffer(new uint8_t[stride]);
        size_t height_half = width / 2;
        for (size_t y = 0; y < height_half; ++y) {
            uint8_t* top = texture_data + y * stride;
            uint8_t* bottom = texture_data + (height - y - 1) * stride;

            memcpy(buffer.get(), top, stride);
            memcpy(top, bottom, stride);
            memcpy(bottom, buffer.get(), stride);
        }
        auto format = components == 3 ? gli::format::FORMAT_RGB8_UNORM_PACK8 : gli::format::FORMAT_RGBA8_UNORM_PACK8;
        gli::texture2d texture(format, gli::extent2d(width, height), 1);

        std::memcpy(texture.data(), texture_data, texture.size());

        auto mipmaped = gli::generate_mipmaps(texture, gli::FILTER_LINEAR);

        FilterProperties props;
        props.magnification_filter = FilterMode::Linear;
        props.minification_filter = FilterMode::LinearMipmapLinear;

        render_texture->initialize(mipmaped, props);

        stbi_image_free(texture_data);
    }

    return render_texture;
}

