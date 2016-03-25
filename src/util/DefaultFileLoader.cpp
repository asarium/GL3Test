//
//

#include "DefaultFileLoader.hpp"

DefaultFileLoader::DefaultFileLoader() : _basepath("resources/") {
}

DefaultFileLoader::DefaultFileLoader(const std::string &basepath) : _basepath(basepath) {
}

DefaultFileLoader::~DefaultFileLoader() {
}

std::vector<uint8_t> DefaultFileLoader::getFileContents(const std::string &filename) {
    auto fullpath = _basepath + filename;

    auto fp = fopen(fullpath.c_str(), "r");

    if (fp == nullptr) {
        return std::vector<uint8_t>();
    }

    std::fseek(fp, 0, SEEK_END);
    auto length = ftell(fp);
    std::fseek(fp, 0, SEEK_SET);

    std::vector<uint8_t> content;
    content.resize(length);
    std::fread(content.data(), 1, length, fp);

    std::fclose(fp);

    return content;
}