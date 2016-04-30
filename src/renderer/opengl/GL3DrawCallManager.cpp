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

GL3DrawCallProperties GL3DrawCallManager::convertProperties(const DrawCallCreateProperties& props) {
    GL3DrawCallProperties gl_props;
    gl_props.vertexLayout = static_cast<GL3VertexLayout*>(props.vertexLayout);
    gl_props.primitive_type = getPrimitiveType(props.primitive_type);

    gl_props.indexed = props.index_type != IndexType::None;
    if (gl_props.indexed) {
        gl_props.index.type = getIndexType(props.index_type);
    }

    return gl_props;
}

std::unique_ptr<DrawCall> GL3DrawCallManager::createDrawCall(const DrawCallCreateProperties& props) {
    GL3DrawCallProperties gl_props = convertProperties(props);

    gl_props.count = static_cast<GLsizei>(props.count);
    gl_props.offset = static_cast<GLint>(props.offset);
    gl_props.instanced = false;

    return std::unique_ptr<DrawCall>(new GL3DrawCall(gl_props));
}

std::unique_ptr<VariableDrawCall> GL3DrawCallManager::createVariableDrawCall(const DrawCallCreateProperties& props) {
    GL3DrawCallProperties gl_props = convertProperties(props);

    gl_props.count = 0;
    gl_props.offset = 0;
    gl_props.instanced = false;

    return std::unique_ptr<VariableDrawCall>(new GL3DrawCall(gl_props));
}

std::unique_ptr<InstancedDrawCall> GL3DrawCallManager::createInstancedDrawCall(const DrawCallCreateProperties& props) {
    GL3DrawCallProperties gl_props = convertProperties(props);

    gl_props.count = static_cast<GLsizei>(props.count);
    gl_props.offset = static_cast<GLint>(props.offset);
    gl_props.instanced = true;

    return std::unique_ptr<InstancedDrawCall>(new GL3DrawCall(gl_props));
}

std::unique_ptr<InstancedVariableDrawCall> GL3DrawCallManager::createInstancedVariableDrawCall(const DrawCallCreateProperties& props) {
    GL3DrawCallProperties gl_props = convertProperties(props);

    gl_props.count = 0;
    gl_props.offset = 0;
    gl_props.instanced = true;

    return std::unique_ptr<InstancedVariableDrawCall>(new GL3DrawCall(gl_props));
}
