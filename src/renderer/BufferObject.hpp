//
//

#ifndef PROJECT_VERTEXBUFFER_H
#define PROJECT_VERTEXBUFFER_H
#pragma once

#include <cstddef>

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

class BufferObject {
public:
    virtual ~BufferObject() {}

    virtual BufferType getType() = 0;

    virtual void setData(const void* data, size_t size, BufferUsage usage) = 0;
};

#endif //PROJECT_VERTEXBUFFER_H
