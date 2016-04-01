# top-level files
set(file_root
        main.cpp
        )

set(file_renderer
        renderer/BufferObject.hpp
        renderer/DrawCall.hpp
        renderer/DrawCallManager.hpp
        renderer/LightingManager.hpp
        renderer/PipelineState.hpp
        renderer/Renderer.hpp
        renderer/ShaderParameters.hpp
        renderer/Texture2D.hpp
        renderer/VertexLayout.hpp
        )

set(file_renderer_opengl
        renderer/opengl/Enums.hpp
        renderer/opengl/EnumTranslation.hpp
        renderer/opengl/GL3BufferObject.cpp
        renderer/opengl/GL3BufferObject.hpp
        renderer/opengl/GL3DrawCall.cpp
        renderer/opengl/GL3DrawCall.hpp
        renderer/opengl/GL3DrawCallManager.cpp
        renderer/opengl/GL3DrawCallManager.hpp
        renderer/opengl/GL3LightingManager.cpp
        renderer/opengl/GL3LightingManager.hpp
        renderer/opengl/GL3PipelineState.cpp
        renderer/opengl/GL3PipelineState.hpp
        renderer/opengl/GL3Renderer.cpp
        renderer/opengl/GL3Renderer.hpp
        renderer/opengl/GL3ShaderDefintions.cpp
        renderer/opengl/GL3ShaderDefintions.hpp
        renderer/opengl/GL3ShaderManager.cpp
        renderer/opengl/GL3ShaderManager.hpp
        renderer/opengl/GL3ShaderParameters.cpp
        renderer/opengl/GL3ShaderParameters.hpp
        renderer/opengl/GL3ShaderProgram.cpp
        renderer/opengl/GL3ShaderProgram.hpp
        renderer/opengl/GL3State.cpp
        renderer/opengl/GL3State.hpp
        renderer/opengl/GL3Texture2D.cpp
        renderer/opengl/GL3Texture2D.hpp
        renderer/opengl/GL3VertexLayout.cpp
        renderer/opengl/GL3VertexLayout.hpp
        )

set(file_test
        test/Application.cpp
        test/Application.hpp
        test/AssimpModel.cpp
        test/AssimpModel.hpp
        test/stb_image.h
        )

set(file_util
        util/DefaultFileLoader.hpp
        util/DefaultFileLoader.cpp
        util/FileLoader.hpp
        util/Timing.hpp
        util/Timing.cpp
        )

# the source groups
source_group("" FILES ${file_root})
source_group("Renderer" FILES ${file_renderer})
source_group("Renderer\\opengl" FILES ${file_renderer_opengl})
source_group("Test" FILES ${file_test})
source_group("Util" FILES ${file_util})

# append all files to the file_root
set(file_root
        ${file_root}
        ${file_renderer}
        ${file_renderer_opengl}
        ${file_test}
        ${file_util}
        )