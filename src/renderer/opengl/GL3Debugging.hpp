#pragma once

#include <renderer/Debugging.hpp>


class GL3Debugging final : public Debugging {
 public:
    ~GL3Debugging() {}

    void addMessage(DebugSeverity severity, const std::string& message) override;

    void pushGroup(const std::string& name) override;
    void popGroup() override;
};


