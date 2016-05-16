#pragma once

#include <cstddef>
#include <gli/texture.hpp>
#include "Enums.hpp"

class TextureHandle {
 protected:
    TextureHandle() { }
 public:
    virtual ~TextureHandle() { }

    virtual int getNanoVGHandle() = 0;
};

enum class WrapBehavior {
    ClampToEdge,
    ClampToBorder
};

enum class TextureCompareMode {
    None,
    CompareRefToTexture
};

struct AllocationProperties {
    gli::target target;
    gli::extent3d size;
    gli::format format;

    WrapBehavior wrap_behavior;
    glm::vec4 border_color;

    TextureCompareMode compare_mode;
    ComparisionFunction compare_func;

    AllocationProperties()
        : wrap_behavior(WrapBehavior::ClampToEdge), border_color(glm::vec4()), compare_mode(TextureCompareMode::None),
          compare_func(ComparisionFunction::Always) { }
};

class Texture: public TextureHandle {
 public:
    virtual ~Texture() { }

    virtual void allocate(const AllocationProperties& props) = 0;

    virtual void initialize(const gli::texture& texture) = 0;
};
