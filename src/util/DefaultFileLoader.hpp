#pragma once

#include "FileLoader.hpp"

#include <string>

class DefaultFileLoader : public FileLoader {
    std::string _basepath;
public:
    DefaultFileLoader();
    DefaultFileLoader(const std::string& basepath);

    ~DefaultFileLoader();

    virtual std::vector<uint8_t> getFileContents(const std::string &filename) override;
};


