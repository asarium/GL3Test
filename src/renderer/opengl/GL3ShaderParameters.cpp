//
//

#include "GL3ShaderParameters.hpp"

#include "EnumTranslation.hpp"
#include "GL3State.hpp"

GL3Descriptor::GL3Descriptor(GL3DescriptorSetPart part) : _active(false) {
    _data.part = part;
}

void GL3Descriptor::setTexture(TextureHandle* handle) {
    if (handle == nullptr) {
        // No texture
        setGLTexture(GL3TextureHandle(GL_TEXTURE_2D, 0));
    } else {
        setGLTexture(*static_cast<GL3Texture*>(handle));
    }
}

void GL3Descriptor::setGLTexture(const GL3TextureHandle& handle) {
    _data.type = DescriptorType::Texture;
    _data.descriptor_data.texture = handle;

    if (_active) {
        // Update bound values
        bind();
    }
}

void GL3Descriptor::setUniformBuffer(BufferObject* object, size_t offset, size_t range) {
    Assertion(object->getType() == BufferType::Uniform, "Buffer must be a uniform buffer!");
    Assertion((offset & GLState->Constants.getUniformBufferAlignment()) == 0,
              "The uniform offset must be properly aligned!");

    _data.type = DescriptorType::UniformBuffer;
    _data.descriptor_data.buffer.buffer = static_cast<GL3BufferObject*>(object);
    _data.descriptor_data.buffer.offset = static_cast<GLintptr>(offset);
    _data.descriptor_data.buffer.size = static_cast<GLsizei>(range);

    if (_active) {
        // Update bound values
        bind();
    }
}

void GL3Descriptor::bind() {
    switch (_data.type) {
        case DescriptorType::UniformBuffer:
            glBindBufferRange(GL_UNIFORM_BUFFER,
                              mapDescriptorSetPartLocation(_data.part),
                              _data.descriptor_data.buffer.buffer->getHandle(),
                              _data.descriptor_data.buffer.offset,
                              _data.descriptor_data.buffer.size);
            break;
        case DescriptorType::Texture:
            _data.descriptor_data.texture.bind(mapDescriptorSetPartLocation(_data.part));
            break;
    }

    _active = true;
}

void GL3Descriptor::unbind() {
    switch (_data.type) {
        case DescriptorType::UniformBuffer:
            break;
        case DescriptorType::Texture:
            // Unbind this texture type
            GLState->Texture.bindTexture(mapDescriptorSetPartLocation(_data.part), GL_TEXTURE_2D, 0);
            break;
    }

    _active = false;
}

GL3DescriptorSet::GL3DescriptorSet(Gl3DescriptorSetType type) : _type(type), _active(false) {
}

Descriptor* GL3DescriptorSet::getDescriptor(DescriptorSetPart part) {
    return getDescriptor(convertDescriptorSetPart(part));
}
GL3Descriptor* GL3DescriptorSet::getDescriptor(GL3DescriptorSetPart part) {
    auto iter = _descriptors.find(part);

    if (iter != _descriptors.end()) {
        return iter->second.get();
    }

    auto result = _descriptors.insert(std::make_pair(part, std::unique_ptr<GL3Descriptor>(new GL3Descriptor(part))));
    return result.first->second.get();
}


void GL3DescriptorSet::bind() {
    Assertion(!_active, "Descriptor set is already bound!");

    for (auto& entry : _descriptors) {
        entry.second->bind();
    }

    _active = true;
}
void GL3DescriptorSet::unbind() {
    Assertion(_active, "Descriptor set wasn't bound when unbind was called!");

    for (auto& entry : _descriptors) {
        entry.second->unbind();
    }

    _active = false;
}
