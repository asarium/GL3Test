#pragma once

#include "renderer/PipelineState.hpp"

class GL3PipelineState : public PipelineState {
    PipelineProperties _props;
public:
    GL3PipelineState(const PipelineProperties &props) : _props(props) { }

    virtual ~GL3PipelineState() { }

    void setupState();
};


