#pragma once

#include "Enums.hpp"

#include <memory>
#include <util/FileLoader.hpp>

#include <glad/glad.h>
#include <renderer/PipelineState.hpp>

class GL3ShaderManager {
    GLuint _programCache[num_enum_values<ShaderType>()];

    FileLoader* _fileLoader;
public:
    GL3ShaderManager(FileLoader *_fileLoader);
    ~GL3ShaderManager();

    GLuint getProgram(ShaderType type);

    void bindProgram(ShaderType type);
};


