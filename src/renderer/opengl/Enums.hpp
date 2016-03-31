#pragma once

#include <stddef.h>

template<typename E>
constexpr size_t num_enum_values() {
    return static_cast<size_t>(E::NUM_VALUES);
}

// This is basically the same as ShaderParameterType but enums can't inherit values from another so they have to be duplicated
enum class GL3ShaderParameterType {
    ModelMatrix = 0,
    ViewMatrix,
    ProjectionMatrix,
    ColorTexture,
    WindowSize,

    NUM_VALUES
};

enum class GL3ShaderType {
    Mesh = 0,

    NUM_VALUES
};
