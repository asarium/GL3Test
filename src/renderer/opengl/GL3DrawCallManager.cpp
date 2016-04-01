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

std::unique_ptr<DrawCall> GL3DrawCallManager::createDrawCall(const DrawCallProperties &props, PrimitiveType type,
                                                             size_t offset, size_t count) {
    GL3DrawCallProperties gl_props = convertProperties(props);

    gl_props.primitive_type = getPrimitiveType(type);
    gl_props.count = static_cast<GLsizei>(count);
    gl_props.offset = static_cast<GLint>(offset);
    gl_props.indexed = false;

    return std::unique_ptr<DrawCall>(new GL3DrawCall(gl_props));
}

std::unique_ptr<DrawCall> GL3DrawCallManager::createIndexedCall(const DrawCallProperties &props, PrimitiveType type,
                                                                size_t offset, size_t count, IndexType indexType) {
    GL3DrawCallProperties gl_props = convertProperties(props);

    gl_props.primitive_type = getPrimitiveType(type);
    gl_props.count = static_cast<GLsizei>(count);
    gl_props.offset = static_cast<GLint>(offset);

    gl_props.indexed = true;
    gl_props.index.type = getIndexType(indexType);

    return std::unique_ptr<DrawCall>(new GL3DrawCall(gl_props));
}

GL3DrawCallProperties GL3DrawCallManager::convertProperties(const DrawCallProperties &props) {
    GL3DrawCallProperties gl_props;
    gl_props.state = static_cast<GL3PipelineState*>(props.state);
    gl_props.vertexLayout = static_cast<GL3VertexLayout *>(props.vertexLayout);
    gl_props.shader = _manager->getShader(convertShaderType(props.shader));

    return gl_props;
}


