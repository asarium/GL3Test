
find_package(SDL2)
find_package(GLM)

include(source_groups.cmake)

add_executable(ogl3_test ${file_root})

target_link_libraries(ogl3_test PRIVATE sdl gl3 glm)
target_compile_features(ogl3_test PRIVATE cxx_auto_type cxx_nullptr)
target_include_directories(ogl3_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")