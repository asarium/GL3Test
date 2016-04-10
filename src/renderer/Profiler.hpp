#pragma once

#include <memory>
#include <string>
#include <vector>

class ProfilingCategory {
public:
    virtual ~ProfilingCategory() {}

    virtual void begin() = 0;

    virtual void end() = 0;
};

struct ProfilingResult {
    const char* name;
    uint64_t gpu_time;
    uint64_t cpu_time;
};

class Profiler {
public:
    virtual ~Profiler() {}

    virtual ProfilingCategory* createCategory(const std::string& name) = 0;

    virtual std::vector<ProfilingResult> getResults() = 0;
};
