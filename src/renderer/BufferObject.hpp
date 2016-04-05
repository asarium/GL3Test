//
//

#ifndef PROJECT_VERTEXBUFFER_H
#define PROJECT_VERTEXBUFFER_H
#pragma once

#include <cstddef>
#include <type_traits>

enum class BufferType {
    None,
    Vertex,
    Index
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

inline UpdateFlags operator|(UpdateFlags lhs, UpdateFlags rhs) {
    typedef std::underlying_type<UpdateFlags>::type int_type;

    return static_cast<UpdateFlags>(static_cast<int_type>(lhs) | static_cast<int_type>(rhs));
}

inline UpdateFlags &operator|=(UpdateFlags &lhs, UpdateFlags rhs) {
    typedef std::underlying_type<UpdateFlags>::type int_type;

    lhs = static_cast<UpdateFlags>(static_cast<int_type>(lhs) | static_cast<int_type>(rhs));

    return lhs;
}

inline bool operator&(UpdateFlags lhs, UpdateFlags rhs) {
    typedef std::underlying_type<UpdateFlags>::type int_type;

    return (static_cast<int_type>(lhs) & static_cast<int_type>(rhs)) != int_type(0);
}

class BufferObject {
public:
    virtual ~BufferObject() { }

    virtual BufferType getType() = 0;

    virtual void setData(const void *data, size_t size, BufferUsage usage) = 0;

    virtual void updateData(const void *data, size_t size, size_t offset, UpdateFlags flags) = 0;
};

#endif //PROJECT_VERTEXBUFFER_H
