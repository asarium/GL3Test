#pragma once
//
//

#include "renderer/ShaderParameters.hpp"

#include "Enums.hpp"
#include "GL3Texture2D.hpp"
#include "GL3BufferObject.hpp"
#include "GL3Util.hpp"

#include <glad/glad.h>

#include <vector>
#include <cstring>
#include <unordered_map>

class GL3Descriptor final: public Descriptor {
public:
    struct Data
    {
        DescriptorType type;
        GL3DescriptorSetPart part;
        struct
        {
            GL3TextureHandle texture;
            struct
            {
                GL3BufferObject* buffer;
                GLintptr offset;
                GLsizei size;
            } buffer;
        } descriptor_data;
    };

private:
    Data _data;
    bool _active;
public:
    explicit GL3Descriptor(GL3DescriptorSetPart part);
    virtual ~GL3Descriptor() {}

    void setTexture(Texture2DHandle* handle) override;

    void setGLTexture(const GL3TextureHandle& handle);

    void setUniformBuffer(BufferObject* object, size_t offset, size_t range) override;

    void bind();

    void unbind();

    const Data& getData() const
    {
        return _data;
    }
};

class GL3DescriptorSet final: public DescriptorSet {
    Gl3DescriptorSetType _type;
    bool _active;

    std::unordered_map<GL3DescriptorSetPart, std::unique_ptr<GL3Descriptor>, EnumClassHash<GL3DescriptorSetPart>> _descriptors;
public:
    explicit GL3DescriptorSet(Gl3DescriptorSetType type);
    virtual ~GL3DescriptorSet() {}

    GL3Descriptor* getDescriptor(GL3DescriptorSetPart part);

    Descriptor* getDescriptor(DescriptorSetPart part) override;

    void bind() override;

    void unbind() override;
};

