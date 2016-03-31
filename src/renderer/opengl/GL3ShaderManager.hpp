#pragma once

#include "Enums.hpp"
#include "GL3ShaderProgram.hpp"

#include <memory>

class GL3ShaderManager {
    std::unique_ptr<GL3ShaderProgram> _programCache[num_enum_values<GL3ShaderType>()];

    FileLoader* _fileLoader;
public:
    GL3ShaderManager(FileLoader *_fileLoader);
    ~GL3ShaderManager();

    GL3ShaderProgram* getShader(GL3ShaderType type);
};


