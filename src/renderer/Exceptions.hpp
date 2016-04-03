#pragma once

#include <stdexcept>

class RendererException : public std::runtime_error {
public:
    RendererException(const std::string & error) : runtime_error(error) { }
};
