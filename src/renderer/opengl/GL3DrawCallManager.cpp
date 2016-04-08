//
//

#include "GL3DrawCallManager.hpp"
#include "GL3DrawCall.hpp"
#include "EnumTranslation.hpp"

namespace {
    GLenum getPrimitiveType(PrimitiveType type) {
        switch (type) {
            case PrimitiveType::Triangle:
                return GL_TRIANGLES;
            case PrimitiveType::Point:
                return GL_POINTS;
            case PrimitiveType::TriangleStrip:
                return GL_TRIANGLE_STRIP;
        }
        return GL_TRIANGLES;
    }

    GLenum getIndexType(IndexType type) {
        switch (type) {
            case IndexType::Short:
                return GL_UNSIGNED_SHORT;
            case IndexType::Integer:
                return GL_UNSIGNED_INT;
        }
        return GL_UNSIGNED_INT;
    }
}

GL3DrawCallProperties GL3DrawCallManager::convertProperties(const DrawCallProperties &props) {
    GL3DrawCallProperties gl_props;
    gl_props.state = static_cast<GL3PipelineState *>(props.state);
    gl_props.vertexLayout = static_cast<GL3VertexLayout *>(props.vertexLayout);

    return gl_props;
}

std::unique_ptr<DrawCall> GL3DrawCallManager::createDrawCall(const DrawCallProperties &props, PrimitiveType type,
                                                             size_t offset, size_t count) {
    GL3DrawCallProperties gl_props = convertProperties(props);

    gl_props.primitive_type = getPrimitiveType(type);
    gl_props.count = static_cast<GLsizei>(count);
    gl_props.offset = static_cast<GLint>(offset);
    gl_props.instanced = false;

    gl_props.indexed = false;

    return std::unique_ptr<DrawCall>(new GL3DrawCall(gl_props));
}

std::unique_ptr<DrawCall> GL3DrawCallManager::createIndexedCall(const DrawCallProperties &props, PrimitiveType type,
                                                                size_t offset, size_t count, IndexType indexType) {
    GL3DrawCallProperties gl_props = convertProperties(props);

    gl_props.primitive_type = getPrimitiveType(type);
    gl_props.count = static_cast<GLsizei>(count);
    gl_props.offset = static_cast<GLint>(offset);
    gl_props.instanced = false;

    gl_props.indexed = true;
    gl_props.index.type = getIndexType(indexType);

    return std::unique_ptr<DrawCall>(new GL3DrawCall(gl_props));
}

std::unique_ptr<VariableDrawCall> GL3DrawCallManager::createVariableDrawCall(const DrawCallProperties &props,
                                                                             PrimitiveType type) {
    GL3DrawCallProperties gl_props = convertProperties(props);

    gl_props.primitive_type = getPrimitiveType(type);
    gl_props.count = 0;
    gl_props.offset = 0;
    gl_props.instanced = false;

    gl_props.indexed = false;

    return std::unique_ptr<VariableDrawCall>(new GL3DrawCall(gl_props));
}

std::unique_ptr<VariableDrawCall> GL3DrawCallManager::createVariableIndexedCall(const DrawCallProperties &props,
                                                                                PrimitiveType type,
                                                                                IndexType indexType) {
    GL3DrawCallProperties gl_props = convertProperties(props);

    gl_props.primitive_type = getPrimitiveType(type);
    gl_props.count = 0;
    gl_props.offset = 0;
    gl_props.instanced = false;

    gl_props.indexed = true;
    gl_props.index.type = getIndexType(indexType);

    return std::unique_ptr<VariableDrawCall>(new GL3DrawCall(gl_props));
}

std::unique_ptr<InstancedDrawCall> GL3DrawCallManager::createInstancedDrawCall(const DrawCallProperties &props, PrimitiveType type,
                                                             size_t offset, size_t count) {
    GL3DrawCallProperties gl_props = convertProperties(props);

    gl_props.primitive_type = getPrimitiveType(type);
    gl_props.count = static_cast<GLsizei>(count);
    gl_props.offset = static_cast<GLint>(offset);
    gl_props.instanced = true;

    gl_props.indexed = false;

    return std::unique_ptr<InstancedDrawCall>(new GL3DrawCall(gl_props));
}

std::unique_ptr<InstancedDrawCall> GL3DrawCallManager::createInstancedIndexedCall(const DrawCallProperties &props, PrimitiveType type,
                                                                size_t offset, size_t count, IndexType indexType) {
    GL3DrawCallProperties gl_props = convertProperties(props);

    gl_props.primitive_type = getPrimitiveType(type);
    gl_props.count = static_cast<GLsizei>(count);
    gl_props.offset = static_cast<GLint>(offset);
    gl_props.instanced = true;

    gl_props.indexed = true;
    gl_props.index.type = getIndexType(indexType);

    return std::unique_ptr<InstancedDrawCall>(new GL3DrawCall(gl_props));
}

std::unique_ptr<InstancedVariableDrawCall> GL3DrawCallManager::createInstancedVariableDrawCall(const DrawCallProperties &props,
                                                                             PrimitiveType type) {
    GL3DrawCallProperties gl_props = convertProperties(props);

    gl_props.primitive_type = getPrimitiveType(type);
    gl_props.count = 0;
    gl_props.offset = 0;
    gl_props.instanced = true;

    gl_props.indexed = false;

    return std::unique_ptr<InstancedVariableDrawCall>(new GL3DrawCall(gl_props));
}

std::unique_ptr<InstancedVariableDrawCall> GL3DrawCallManager::createInstancedVariableIndexedCall(const DrawCallProperties &props,
                                                                                PrimitiveType type,
                                                                                IndexType indexType) {
    GL3DrawCallProperties gl_props = convertProperties(props);

    gl_props.primitive_type = getPrimitiveType(type);
    gl_props.count = 0;
    gl_props.offset = 0;
    gl_props.instanced = true;

    gl_props.indexed = true;
    gl_props.index.type = getIndexType(indexType);

    return std::unique_ptr<InstancedVariableDrawCall>(new GL3DrawCall(gl_props));
}
