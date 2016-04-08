#pragma once

#include "ShaderParameters.hpp"

class GenericDrawCall {
public:
    virtual ~GenericDrawCall() { }

    virtual ShaderParameters* getParameters() = 0;
};

class DrawCall : public GenericDrawCall {
public:
    virtual ~DrawCall() { }

    virtual void draw() = 0;
};

class VariableDrawCall : public GenericDrawCall {
public:
    virtual ~VariableDrawCall() { }

    virtual void draw(size_t count, size_t offset) = 0;
};

class InstancedDrawCall : public GenericDrawCall {
public:
    virtual ~InstancedDrawCall() { }

    virtual void drawInstanced(size_t num_instances) = 0;
};

class InstancedVariableDrawCall : public GenericDrawCall {
public:
    virtual ~InstancedVariableDrawCall() { }

    virtual void drawInstanced(size_t num_instances, size_t count, size_t offset) = 0;
};

