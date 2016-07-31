//
//

#ifndef PROJECT_VERTEXLAYOUT_HPP
#define PROJECT_VERTEXLAYOUT_HPP

#include <cstdlib>
#include <memory>
#include <vector>
#include "BufferObject.hpp"

enum class AttributeType {
    Position,
    TexCoord,
    Normal,
    Color,
    Tangent,
    Bitangent,
    Radius,
    PositionOffset,
    Position2D
};

enum class DataFormat {
    Vec4,
    Vec3,
    Vec2,
    Float
};

struct VertexAttributeProperties {
    AttributeType type;
    uint32_t bufferBinding;
    DataFormat format;
    size_t offset;

    VertexAttributeProperties()
        : type(AttributeType::Position), bufferBinding(0), format(DataFormat::Float), offset(0) {}
};

struct VertexBufferBindingProperties {
    uint32_t bufferBinding;
    bool instanced;
    size_t stride;

    VertexBufferBindingProperties() : bufferBinding(0), instanced(false), stride(0) {}
};

struct VertexInputStateProperties {
    std::vector<VertexAttributeProperties> components;
    std::vector<VertexBufferBindingProperties> bufferBindings;

    void addComponent(AttributeType type, uint32_t bufferBinding, DataFormat format, size_t offset);
    void addBufferBinding(uint32_t buffer, bool instanced, size_t stride);
};

inline void VertexInputStateProperties::addComponent(AttributeType type, uint32_t bufferBinding, DataFormat format, size_t offset)
{
    VertexAttributeProperties props;
    props.type = type;
    props.bufferBinding = bufferBinding;
    props.format = format;
    props.offset = offset;

    components.push_back(props);
}

inline void VertexInputStateProperties::addBufferBinding(uint32_t buffer, bool instanced, size_t stride)
{
    VertexBufferBindingProperties props;
    props.bufferBinding = buffer;
    props.instanced = instanced;
    props.stride = stride;

    bufferBindings.push_back(props);
}

class VertexArrayObject {
 public:
    virtual ~VertexArrayObject() {}
};

struct VertexBufferBinding {
    uint32_t binding;
    BufferObject* buffer;

    VertexBufferBinding() : binding(0), buffer(nullptr) {}
};

enum class IndexType {
    Short,
    Integer
};

struct VertexArrayProperties {
    std::vector<VertexBufferBinding> bufferBindings;

    BufferObject* indexBuffer;
    IndexType indexType;
    uint64_t indexOffset;

    VertexArrayProperties() : indexBuffer(nullptr), indexType(IndexType::Short), indexOffset(0) {}

    void addBufferBinding(uint32_t binding, BufferObject* buffer);
};

inline void VertexArrayProperties::addBufferBinding(uint32_t binding, BufferObject* buffer)
{
    VertexBufferBinding bufferBinding;
    bufferBinding.binding = binding;
    bufferBinding.buffer = buffer;

    bufferBindings.push_back(bufferBinding);
}

class VertexInputState {
 public:
    virtual ~VertexInputState() {}

    virtual std::unique_ptr<VertexArrayObject> createArrayObject(const VertexArrayProperties& props) = 0;
};

#endif //PROJECT_VERTEXLAYOUT_HPP
