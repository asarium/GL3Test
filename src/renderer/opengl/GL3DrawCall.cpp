//
//

#include <util/Assertion.hpp>
#include "GL3DrawCall.hpp"
#include "GL3DrawCallManager.hpp"
#include "GL3State.hpp"
#include "GL3Renderer.hpp"

namespace {
size_t getTypeSize(GLenum type) {
    switch (type) {
        case GL_UNSIGNED_SHORT:
            return sizeof(GLshort);
        case GL_UNSIGNED_INT:
            return sizeof(GLint);
        default:
            return sizeof(GLint);
    }
}
}

GL3DrawCall::GL3DrawCall(GL3Renderer* renderer, const GL3DrawCallProperties& props)
    : GL3Object(renderer), _properties(props) {
}

GL3DrawCall::~GL3DrawCall() {
    // For OpenGL there is nothing to deallocate
}

void GL3DrawCall::setGLState() {
    _properties.vertexLayout->bind();

    if (_pushConstants.getSize() > 0) {
        _renderer->getPushConstantManager()->setConstants(_pushConstants.getData(), _pushConstants.getSize());
    }
}

void GL3DrawCall::draw() {
    actualDraw(_properties.count, _properties.offset);
}

void GL3DrawCall::draw(size_t count, size_t offset) {
    actualDraw((GLsizei) count, (GLint) offset);
}

void GL3DrawCall::actualDraw(GLsizei count, GLint offset) {
    Assertion(!_properties.instanced, "Instanced draw call executed without instancing!");

    setGLState();

    if (_properties.indexed) {
        auto has_base = _properties.hasBaseVertex();
        auto has_range = _properties.hasRange();
        auto indices = reinterpret_cast<void*>(offset * getTypeSize(_properties.index.type));

        if (has_base && has_range) {
            // Full combination, base vertex and range specified
            glDrawRangeElementsBaseVertex(_properties.primitive_type,
                                          _properties.range_begin,
                                          _properties.range_end,
                                          _properties.count,
                                          _properties.index.type,
                                          indices,
                                          _properties.base_vertex);
        } else if (!has_base && has_range) {
            // Only have a range
            glDrawRangeElements(_properties.primitive_type,
                                _properties.range_begin,
                                _properties.range_end,
                                _properties.count,
                                _properties.index.type,
                                indices);
        } else if (has_base && !has_range) {
            // Only have a base vertex
            glDrawElementsBaseVertex(_properties.primitive_type,
                                     _properties.count,
                                     _properties.index.type,
                                     indices,
                                     _properties.base_vertex);
        } else {
            // We have neither, use the standard way
            glDrawElements(_properties.primitive_type, count, _properties.index.type, indices);
        }
    } else {
        glDrawArrays(_properties.primitive_type, offset, count);
    }
}

void GL3DrawCall::drawInstanced(size_t num_instances) {
    actualDrawInstanced(static_cast<GLsizei>(num_instances), _properties.count, _properties.offset);
}

void GL3DrawCall::drawInstanced(size_t num_instances, size_t count, size_t offset) {
    actualDrawInstanced(static_cast<GLsizei>(num_instances), (GLsizei) count, (GLint) offset);
}

void GL3DrawCall::actualDrawInstanced(GLsizei instances, GLsizei count, GLint offset) {
    Assertion(_properties.instanced, "Non-Instanced draw call executed with instancing!");
    setGLState();

    if (_properties.indexed) {
        auto has_base = _properties.hasBaseVertex();
        auto has_range = _properties.hasRange();
        auto indices = reinterpret_cast<void*>(offset * getTypeSize(_properties.index.type));

        // There are no glDrawRangeElements* variants for instanced rendering, just use the normal functions

        if (has_base && has_range) {
            // Full combination, base vertex and range specified
            glDrawElementsInstancedBaseVertex(_properties.primitive_type,
                                              _properties.count,
                                              _properties.index.type,
                                              indices,
                                              _properties.base_vertex,
                                              instances);
        } else if (!has_base && has_range) {
            // Only have a range
            glDrawElementsInstanced(_properties.primitive_type, count, _properties.index.type, indices, instances);
        } else if (has_base && !has_range) {
            // Only have a base vertex
            glDrawElementsInstancedBaseVertex(_properties.primitive_type,
                                              _properties.count,
                                              _properties.index.type,
                                              indices,
                                              _properties.base_vertex,
                                              instances);
        } else {
            // We have neither, use the standard way
            glDrawElementsInstanced(_properties.primitive_type, count, _properties.index.type, indices, instances);
        }
    } else {
        glDrawArraysInstanced(_properties.primitive_type, offset, count, instances);
    }
}
void GL3DrawCall::setPushConstants(const void* data, size_t size) {
    _pushConstants.updateData(data, size);
}
