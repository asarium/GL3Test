#pragma once

#include <cstddef>
#include <gli/texture.hpp>
#include "Enums.hpp"

class TextureHandle {
 protected:
    TextureHandle() { }
 public:
    virtual ~TextureHandle() { }
};

enum class WrapBehavior {
    ClampToEdge,
    ClampToBorder,
    Repeat
};

enum class TextureCompareMode {
    None,
    CompareRefToTexture
};

enum class FilterMode {
    Nearest,
    Linear,
    NearestMipmapNearest,
    LinearMipmapNearest,
    NearestMipmapLinear,
    LinearMipmapLinear
};

struct FilterProperties {
    WrapBehavior wrap_behavior_s;
    WrapBehavior wrap_behavior_t;
    WrapBehavior wrap_behavior_r;
    glm::vec4 border_color;

    FilterMode minification_filter;
    FilterMode magnification_filter;

    FilterProperties()
        : wrap_behavior_s(WrapBehavior::ClampToEdge), wrap_behavior_t(WrapBehavior::ClampToEdge),
          wrap_behavior_r(WrapBehavior::ClampToEdge), border_color(glm::vec4(1.f)),
          minification_filter(FilterMode::Linear), magnification_filter(FilterMode::Linear) { }
};

struct AllocationProperties {
    gli::target target;
    gli::extent3d size;
    gli::format format;

    FilterProperties filterProperties;

    TextureCompareMode compare_mode;
    ComparisionFunction compare_func;

    AllocationProperties()
        : target(gli::TARGET_FIRST), format(gli::FORMAT_FIRST), compare_mode(TextureCompareMode::None), compare_func(ComparisionFunction::Always) { }
};

class Texture: public TextureHandle {
 public:
    virtual ~Texture() { }

    virtual void allocate(const AllocationProperties& props) = 0;

    virtual void initialize(const gli::texture& texture, const FilterProperties& filterProperties) = 0;

    virtual void update(const gli::extent3d& position,
                        const gli::extent3d& size, const gli::format dataFormat, const void* data) = 0;

    virtual gli::extent3d getSize() const = 0;
};
