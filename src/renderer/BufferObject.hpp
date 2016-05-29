//
//

#ifndef PROJECT_VERTEXBUFFER_H
#define PROJECT_VERTEXBUFFER_H
#pragma once

#include <cstddef>

#include "Enums.hpp"

#include <util/EnumClassUtil.hpp>

enum class BufferType {
    None,
    Vertex,
    Index,
    Uniform
};

enum class BufferUsage {
    Static,
    Dynamic,
    Streaming
};

enum class UpdateFlags {
    None = 0,
    DiscardOldData = 1 << 0
};
template<>
struct BitOperationsTag<UpdateFlags> {
    static constexpr bool value = true;
};

enum class BufferMapFlags {
    None = 0,
    Read = 1 << 0,
    Write = 1 << 1,
    InvalidateData = 1 << 2,
};
template<>
struct BitOperationsTag<BufferMapFlags> {
    static constexpr bool value = true;
};


class BufferObject {
public:
    virtual ~BufferObject() { }

    virtual BufferType getType() const = 0;

    virtual void setData(const void *data, size_t size, BufferUsage usage) = 0;

    virtual void updateData(const void *data, size_t offset, size_t size,UpdateFlags flags) = 0;

    virtual void* map(size_t offset, size_t size, BufferMapFlags flags) = 0;

    virtual bool unmap() = 0;
};

#endif //PROJECT_VERTEXBUFFER_H
