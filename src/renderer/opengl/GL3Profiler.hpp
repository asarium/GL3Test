#pragma once

#include <renderer/Profiler.hpp>
#include "GL3Object.hpp"

#include <glad/glad.h>

#include <queue>
#include <vector>

class GL3Profiler;

class GL3ProfilingCategory : public ProfilingCategory
{
    struct QueryPair {
        GLuint begin_query;
        GLuint end_query;

        uint64_t cpu_time;
    };

    std::string _name;
    GL3Profiler* _profiler;

    std::queue<QueryPair> _queryPairs;

    uint64_t _cpuBeginTime;
    GLuint _currentBeginQuery;
public:
    GL3ProfilingCategory(const std::string& name, GL3Profiler* profiler);

    ~GL3ProfilingCategory()
    {
    }

    const std::string& getName() const {
        return _name;
    }

    void begin() override;

    void end() override;

    bool hasTimes();

    std::pair<uint64_t, uint64_t> getTimes();
};

class GL3Profiler : public Profiler, public GL3Object
{
    std::vector<GLuint> _queryObjects;
    std::vector<GLuint> _freeQueryObjects;

    std::vector<std::unique_ptr<GL3ProfilingCategory>> _categories;

    uint64_t _cpuTimeFrequency;
public:
    GL3Profiler(GL3Renderer* renderer);

    virtual ~GL3Profiler();

    ProfilingCategory* createCategory(const std::string& name) override;

    std::vector<ProfilingResult> getResults() override;

    GLuint getQueryObject();

    void freeQueryObject(GLuint query);
};

