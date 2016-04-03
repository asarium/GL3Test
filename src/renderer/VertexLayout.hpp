//
//

#ifndef PROJECT_VERTEXLAYOUT_HPP
#define PROJECT_VERTEXLAYOUT_HPP

#include <cstdlib>
#include <memory>
#include "BufferObject.hpp"

enum class AttributeType {
    Position,
    TexCoord,
    Normal,
    Color,
    Tangent,
};

enum class DataFormat {
    Vec4,
    Vec3,
    Vec2
};

class VertexLayout {
public:
    typedef size_t BufferIndex;

    virtual ~VertexLayout() {}

    virtual BufferIndex attachBufferObject(BufferObject *buffer) = 0;

    virtual void addComponent(AttributeType type, DataFormat format, size_t stride, BufferIndex source, size_t offset) = 0;

    virtual void setIndexBuffer(BufferIndex source) = 0;

    virtual void finalize() = 0;
};

#endif //PROJECT_VERTEXLAYOUT_HPP
