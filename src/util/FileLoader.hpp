//
//

#ifndef PROJECT_FILELOADER_HPP
#define PROJECT_FILELOADER_HPP

#include <cstdint>
#include <vector>
#include <string>

class FileLoader {
public:
    virtual ~FileLoader() {}

    virtual std::vector<uint8_t> getFileContents(const std::string& filename) = 0;
};

#endif //PROJECT_FILELOADER_HPP
