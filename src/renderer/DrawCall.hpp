#pragma once

#include "ShaderParameters.hpp"

class GenericDrawCall {
public:
    virtual ~GenericDrawCall() { }

    virtual void setPushConstants(const void* data, size_t size) = 0;
};

class DrawCall : public GenericDrawCall {
public:
    virtual ~DrawCall() { }

    virtual void draw() = 0;
};

class VariableDrawCall : public GenericDrawCall {
public:
    virtual ~VariableDrawCall() { }

    virtual void draw(size_t offset, size_t count) = 0;
};

class InstancedDrawCall : public GenericDrawCall {
public:
    virtual ~InstancedDrawCall() { }

    virtual void drawInstanced(size_t num_instances) = 0;
};

class InstancedVariableDrawCall : public GenericDrawCall {
public:
    virtual ~InstancedVariableDrawCall() { }

    virtual void drawInstanced(size_t num_instances, size_t offset, size_t count) = 0;
};

