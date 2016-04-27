//
//

#include "GL3ShaderProgram.hpp"
#include "GL3VertexLayout.hpp"
#include "GL3ShaderParameters.hpp"
#include "GL3ShaderDefintions.hpp"
#include "GL3State.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <renderer/VertexLayout.hpp>
#include <util/Assertion.hpp>

GL3ShaderProgram::GL3ShaderProgram(GLuint handle, const GL3ShaderDefinition& definition) : _handle(handle) {
    // Now get the locations of the uniformss
    for (auto& mapping : definition.uniforms) {
        auto loc = glGetUniformLocation(_handle, mapping.name);

        _uniformLocations[static_cast<size_t>(mapping.parameter)] = loc;
    }
}

GL3ShaderProgram::~GL3ShaderProgram() {
    glDeleteProgram(_handle);
}

void GL3ShaderProgram::bind() {
    GLState->Program.use(this);
}

void GL3ShaderProgram::bindAndSetParameters(const GL3ShaderParameters* parameters) {
    this->bind();
    GLint texture_unit = 0;

    GLState->Texture.unbindAll();

    for (auto& parameter : parameters->getValues()) {

        auto uniform_loc = getUniformLocation(parameter.param_type);

        if (uniform_loc < 0) {
            // Uniform isn't used in this shader
            continue;
        }

        switch (parameter.data_type) {
            case ParameterDataType::Vec2:
                glUniform2fv(uniform_loc, 1, glm::value_ptr(parameter.value.vec2));
                break;
            case ParameterDataType::Vec3:
                glUniform3fv(uniform_loc, 1, glm::value_ptr(parameter.value.vec3));
                break;
            case ParameterDataType::Mat4:
                glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(parameter.value.mat4));
                break;
            case ParameterDataType::Tex2D:
                parameter.value.tex2d->bind(texture_unit);

                glUniform1i(uniform_loc, texture_unit);

                ++texture_unit;
                break;
            case ParameterDataType::Tex2DHandle:
                GLState->Texture.bindTexture(texture_unit, GL_TEXTURE_2D, parameter.value.tex2dhandle);

                glUniform1i(uniform_loc, texture_unit);

                ++texture_unit;
                break;
            case ParameterDataType::Integer:
                glUniform1i(uniform_loc, parameter.value.integer);
                break;
            case ParameterDataType::Float:
                glUniform1f(uniform_loc, parameter.value.floatVal);
                break;
        }
    }
}

