#pragma once

struct PipelineProperties {
    bool depth_test;
};

class PipelineState {
public:
    virtual ~PipelineState() { }
};
