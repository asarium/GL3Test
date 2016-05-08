//
//

#include "GL3ShaderParameters.hpp"
#include "GL3ShaderProgram.hpp"

#include "EnumTranslation.hpp"
#include "GL3State.hpp"

GL3ShaderParameters::GL3ShaderParameters() {
}

GL3ShaderParameters::~GL3ShaderParameters() {

}

ParameterValue& GL3ShaderParameters::getValue(GL3ShaderParameterType type) {
    for (auto& entry : _values) {
        if (entry.param_type == type) {
            return entry;
        }
    }
    _values.emplace_back();
    return _values.back();
}

void GL3ShaderParameters::set2dTextureHandle(GL3ShaderParameterType param, GLuint handle) {
    auto& val = getValue(param);
    val.param_type = param;
    val.data_type = ParameterDataType::Tex2DHandle;
    val.value.tex2dhandle = handle;
}

void GL3ShaderParameters::setInteger(GL3ShaderParameterType param, GLint value) {
    auto& val = getValue(param);
    val.param_type = param;
    val.data_type = ParameterDataType::Integer;
    val.value.integer = value;
}

void GL3ShaderParameters::setFloat(GL3ShaderParameterType param, GLfloat value) {
    auto& val = getValue(param);
    val.param_type = param;
    val.data_type = ParameterDataType::Float;
    val.value.floatVal = value;
}

void GL3ShaderParameters::setVec3(GL3ShaderParameterType param, const glm::vec3& value) {
    auto& val = getValue(param);
    val.param_type = param;
    val.data_type = ParameterDataType::Vec3;
    val.value.vec3 = value;
}

void GL3ShaderParameters::setVec2(GL3ShaderParameterType param, const glm::vec2& value) {
    auto& val = getValue(param);
    val.param_type = param;
    val.data_type = ParameterDataType::Vec2;
    val.value.vec2 = value;
}

void GL3ShaderParameters::setMat4(GL3ShaderParameterType param, const glm::mat4& value) {
    auto& val = getValue(param);
    val.param_type = param;
    val.data_type = ParameterDataType::Mat4;
    val.value.mat4 = value;
}

void GL3ShaderParameters::setTexture(GL3ShaderParameterType param, GL3Texture2D* value) {
    auto& val = getValue(param);
    val.param_type = param;
    val.data_type = ParameterDataType::Tex2D;
    val.value.tex2d = value;
}

const std::vector<ParameterValue>& GL3ShaderParameters::getValues() const {
    return _values;
}

void GL3ShaderParameters::setInteger(ShaderParameterType param, int value) {
    setInteger(convertParameterType(param), value);
}

void GL3ShaderParameters::setFloat(ShaderParameterType param, float value) {
    setFloat(convertParameterType(param), value);
}

void GL3ShaderParameters::setVec3(ShaderParameterType param, const glm::vec3& value) {
    setVec3(convertParameterType(param), value);
}

void GL3ShaderParameters::setVec2(ShaderParameterType param, const glm::vec2& value) {
    setVec2(convertParameterType(param), value);
}

void GL3ShaderParameters::setMat4(ShaderParameterType param, const glm::mat4& value) {
    setMat4(convertParameterType(param), value);
}

void GL3ShaderParameters::setTexture(ShaderParameterType param, Texture2DHandle* value) {
    setTexture(convertParameterType(param), static_cast<GL3Texture2D*>(value));
}
void GL3ShaderParameters::setBoolean(ShaderParameterType param, bool value) {
    setInteger(convertParameterType(param), value ? GL_TRUE : GL_FALSE);
}

GL3Descriptor::GL3Descriptor(GL3DescriptorSetPart part) : _active(false) {
    _data.part = part;
}

void GL3Descriptor::setTexture(Texture2DHandle* handle) {
    setGLTexture(*static_cast<GL3Texture2D*>(handle));
}

void GL3Descriptor::setGLTexture(const GL3TextureHandle& handle)
{
    _data.type = DescriptorType::Texture;
    _data.descriptor_data.texture = handle;

    if (_active)
    {
        // Update bound values
        bind();
    }
}

void GL3Descriptor::setUniformBuffer(BufferObject* object, size_t offset, size_t range) {
    Assertion(object->getType() == BufferType::Uniform, "Buffer must be a uniform buffer!");
    Assertion((offset & GLState->Constants.getUniformBufferAlignment()) == 0, "The uniform offset must be properly aligned!");

    _data.type = DescriptorType::UniformBuffer;
    _data.descriptor_data.buffer.buffer = static_cast<GL3BufferObject*>(object);
    _data.descriptor_data.buffer.offset = static_cast<GLintptr>(offset);
    _data.descriptor_data.buffer.size = static_cast<GLsizei>(range);

    if (_active)
    {
        // Update bound values
        bind();
    }
}

void GL3Descriptor::bind()
{
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

void GL3Descriptor::unbind()
{
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
    auto iter  = _descriptors.find(part);

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
