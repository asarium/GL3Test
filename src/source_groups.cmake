# top-level files
set(file_root
    main.cpp
    )

set(file_model
    model/AssimpModelConverter.cpp
    model/AssimpModelConverter.hpp
    model/Model.cpp
    model/Model.hpp
    model/ModelLoader.cpp
    model/ModelLoader.hpp
    )

set(file_renderer
    renderer/BufferObject.hpp
    renderer/CommandBuffer.hpp
    renderer/Enums.hpp
    renderer/Exceptions.hpp
    renderer/PipelineState.hpp
    renderer/Profiler.hpp
    renderer/Renderer.hpp
    renderer/RenderTarget.hpp
    renderer/RenderTargetManager.hpp
    renderer/ShaderParameters.hpp
    renderer/Texture.hpp
    renderer/Util.hpp
    renderer/VertexLayout.hpp
    )

set(file_renderer_nanovg
    renderer/nanovg/fontstash.h
    renderer/nanovg/nanovg.c
    renderer/nanovg/nanovg.h
    renderer/nanovg/nanovg_gl.h
    renderer/nanovg/nanovg_gl_utils.h
    renderer/nanovg/stb_image.h
    renderer/nanovg/stb_truetype.h
    )

set(file_renderer_opengl
    renderer/opengl/Enums.hpp
    renderer/opengl/EnumTranslation.hpp
    renderer/opengl/GL3BufferObject.cpp
    renderer/opengl/GL3BufferObject.hpp
    renderer/opengl/GL3CommandBuffer.hpp
    renderer/opengl/GL3CommandBuffer.cpp
    renderer/opengl/GL3Object.cpp
    renderer/opengl/GL3Object.hpp
    renderer/opengl/GL3PipelineState.cpp
    renderer/opengl/GL3PipelineState.hpp
    renderer/opengl/GL3Profiler.cpp
    renderer/opengl/GL3Profiler.hpp
    renderer/opengl/GL3PushConstantManager.cpp
    renderer/opengl/GL3PushConstantManager.hpp
    renderer/opengl/GL3Renderer.cpp
    renderer/opengl/GL3Renderer.hpp
    renderer/opengl/GL3RenderTarget.cpp
    renderer/opengl/GL3RenderTarget.hpp
    renderer/opengl/GL3RenderTargetManager.cpp
    renderer/opengl/GL3RenderTargetManager.hpp
    renderer/opengl/GL3ShaderDefintions.cpp
    renderer/opengl/GL3ShaderDefintions.hpp
    renderer/opengl/GL3ShaderManager.cpp
    renderer/opengl/GL3ShaderManager.hpp
    renderer/opengl/GL3ShaderParameters.cpp
    renderer/opengl/GL3ShaderParameters.hpp
    renderer/opengl/GL3State.cpp
    renderer/opengl/GL3State.hpp
    renderer/opengl/GL3Texture.cpp
    renderer/opengl/GL3Texture.hpp
    renderer/opengl/GL3Util.cpp
    renderer/opengl/GL3Util.hpp
    renderer/opengl/GL3VertexLayout.cpp
    renderer/opengl/GL3VertexLayout.hpp
    )

set(file_test
    test/Application.cpp
    test/Application.hpp
    test/DrawCall.cpp
    test/DrawCall.hpp
    test/DrawUtil.cpp
    test/DrawUtil.hpp
    test/Light.cpp
    test/Light.hpp
    test/LightingManager.cpp
    test/LightingManager.hpp
    test/NanoVGRenderer.cpp
    test/NanoVGRenderer.hpp
    )

set(file_util
    util/Assertion.hpp
    util/DefaultFileLoader.hpp
    util/DefaultFileLoader.cpp
    util/EnumClassUtil.hpp
    util/FileLoader.hpp
    util/HashUtil.hpp
    util/textures.hpp
    util/textures.cpp
    util/Timing.hpp
    util/Timing.cpp
    util/UniformAligner.hpp
    util/UniqueHandle.hpp
    util/VariableStackArray.hpp
    util/VariableUniformBuffer.cpp
    util/VariableUniformBuffer.hpp
    util/stb_image.h
    )

# the source groups
source_group("" FILES ${file_root})
source_group("Model" FILES ${file_model})
source_group("Renderer" FILES ${file_renderer})
source_group("Renderer\\nanovg" FILES ${file_renderer_nanovg})
source_group("Renderer\\opengl" FILES ${file_renderer_opengl})
source_group("Test" FILES ${file_test})
source_group("Util" FILES ${file_util})

# append all files to the file_root
set(file_root
    ${file_root}
    ${file_model}
    ${file_renderer}
    ${file_renderer_nanovg}
    ${file_renderer_opengl}
    ${file_test}
    ${file_util}
    )
