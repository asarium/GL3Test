//
//

#include "GL3ShaderManager.hpp"


GL3ShaderManager::GL3ShaderManager(FileLoader *fileLoader) : _fileLoader(fileLoader) {
}

GL3ShaderManager::~GL3ShaderManager() {
}

GL3ShaderProgram *GL3ShaderManager::getShader(GL3ShaderType type) {
    if (_programCache[static_cast<size_t>(type)]) {
        return _programCache[static_cast<size_t>(type)].get();
    }

    _programCache[static_cast<size_t>(type)].reset(new GL3ShaderProgram(_fileLoader, getShaderDefinition(type)));

    return _programCache[static_cast<size_t>(type)].get();
}

