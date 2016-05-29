#pragma once

#include "Enums.hpp"

#include <memory>
#include <util/FileLoader.hpp>

#include <glad/glad.h>
#include <renderer/PipelineState.hpp>
#include <unordered_map>

#include <util/HashUtil.hpp>

class GL3ShaderManager {
    std::unordered_map<std::pair<ShaderType, ShaderFlags>, GLuint> _programCache;

    FileLoader* _fileLoader;
public:
    GL3ShaderManager(FileLoader *_fileLoader);
    ~GL3ShaderManager();

    GLuint getProgram(ShaderType type, ShaderFlags flags);
};


