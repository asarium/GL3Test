#pragma once

#include <string>

enum class DebugSeverity {
    Low,
    Medium,
    High
};

class Debugging {
 public:
    virtual ~Debugging() {}

    virtual void addMessage(DebugSeverity severity, const std::string& message) = 0;

    virtual void pushGroup(const std::string& name) = 0;
    virtual void popGroup() = 0;
};

class GroupScope {
    Debugging* _debug;
 public:
    explicit GroupScope(Debugging* debug, const std::string& name) : _debug(debug) {
        debug->pushGroup(name);
    }
    ~GroupScope() {
        _debug->popGroup();
    }
};

#ifndef NDEBUG
#define DEBUG_SCOPE(varname, debug, message) GroupScope varname(debug, message)
#else
#define DEBUG_SCOPE(varname, debug, message)
#endif

