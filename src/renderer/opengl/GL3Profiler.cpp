#include <util/Assertion.hpp>
#include "GL3Profiler.hpp"
#include "GL3Renderer.hpp"

#include <SDL_timer.h>

GL3ProfilingCategory::GL3ProfilingCategory(const std::string &name, GL3Profiler *profiler) : _name(name),
                                                                                             _profiler(profiler),
                                                                                             _currentBeginQuery(0) {

}

void GL3ProfilingCategory::begin() {
    Assertion(_currentBeginQuery == 0, "begin() was called without a matching call to end()!");

    // Get a query object from the profiler
    _currentBeginQuery = _profiler->getQueryObject();

    glQueryCounter(_currentBeginQuery, GL_TIMESTAMP);
    _cpuBeginTime = SDL_GetPerformanceCounter();
}

void GL3ProfilingCategory::end() {
    Assertion(_currentBeginQuery != 0, "begin() was not called!");
    auto endQuery = _profiler->getQueryObject();
    glQueryCounter(endQuery, GL_TIMESTAMP);

    QueryPair pair;
    pair.begin_query = _currentBeginQuery;
    pair.end_query = endQuery;
    pair.cpu_time = SDL_GetPerformanceCounter() - _cpuBeginTime;
    _queryPairs.push(pair);

    // Reset begin query again
    _currentBeginQuery = 0;
    _cpuBeginTime = 0;
}

bool GL3ProfilingCategory::hasTimes() {
    if (_queryPairs.empty()) {
        return false;
    }

    auto &frontPair = _queryPairs.front();
    GLint resAvailable;
    glGetQueryObjectiv(frontPair.end_query, GL_QUERY_RESULT_AVAILABLE, &resAvailable);

    return resAvailable == GL_TRUE;
}

std::pair<uint64_t, uint64_t> GL3ProfilingCategory::getTimes() {
    std::pair<uint64_t, uint64_t> result_pair;
    while(hasTimes()) {
        // Remove all finished pairs from the queue and return the result of the last finished pair
        auto frontPair = _queryPairs.front();
        _queryPairs.pop();

        GLint64 beginTime;
        GLint64 endTime;
        glGetQueryObjecti64v(frontPair.begin_query, GL_QUERY_RESULT, &beginTime);
        glGetQueryObjecti64v(frontPair.end_query, GL_QUERY_RESULT, &endTime);

        auto gpuTime = endTime - beginTime; // This is in nano-seconds
        result_pair.first = (uint64_t) gpuTime;
        result_pair.second = frontPair.cpu_time; // This is stored in a SDL specific resolution

        // Return the query objects back to the profiler so they can be reused
        _profiler->freeQueryObject(frontPair.begin_query);
        _profiler->freeQueryObject(frontPair.end_query);
    }

    return result_pair;
}


GL3Profiler::GL3Profiler(GL3Renderer *renderer) : GL3Object(renderer) {
    _queryObjects.resize(10);
    glGenQueries(10, &_queryObjects[0]);

    _freeQueryObjects = _queryObjects; // Initially all queries are free

    _cpuTimeFrequency = SDL_GetPerformanceFrequency();
}

GL3Profiler::~GL3Profiler() {
    glDeleteQueries((GLsizei) _queryObjects.size(), &_queryObjects[0]);
}

ProfilingCategory *GL3Profiler::createCategory(const std::string &name) {
    _categories.emplace_back(new GL3ProfilingCategory(name, this));
    return _categories.back().get();
}


GLuint GL3Profiler::getQueryObject() {
    if (!_freeQueryObjects.empty()) {
        // Always use the last query in the free list. That saves us from having to implement
        // a sliding window or something like that.
        auto query = _freeQueryObjects.back();
        _freeQueryObjects.pop_back();

        return query;
    }

    // No free query, add a new one
    GLuint newQuery;
    glGenQueries(1, &newQuery);
    _queryObjects.push_back(newQuery);

    // Don't add this one to the free list because it's going to be used immediately.
    return newQuery;
}

void GL3Profiler::freeQueryObject(GLuint query) {
    _freeQueryObjects.push_back(query);
}

std::vector<ProfilingResult> GL3Profiler::getResults() {
    std::vector<ProfilingResult> results;

    for (auto& category : _categories) {
        if (category->hasTimes()) {
            auto times = category->getTimes();

            ProfilingResult result;
            result.name = category->getName().c_str();
            result.gpu_time = times.first;
            result.cpu_time = (times.second * 1000000000) / _cpuTimeFrequency; // This is also stored in nanoseconds

            results.push_back(result);
        }
    }

    return results;
}

