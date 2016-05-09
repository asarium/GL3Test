#pragma once

#include "Enums.hpp"

#include <memory>
#include <util/FileLoader.hpp>

#include <glad/glad.h>

class GL3ShaderManager {
    GLuint _programCache[num_enum_values<GL3ShaderType>()];

    FileLoader* _fileLoader;
public:
    GL3ShaderManager(FileLoader *_fileLoader);
    ~GL3ShaderManager();

    GLuint getProgram(GL3ShaderType type);

    void bindProgram(GL3ShaderType type);
};


