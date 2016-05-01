
find_package(SDL2 REQUIRED)
find_package(GLM REQUIRED)
find_package(assimp REQUIRED)
find_package(Jansson REQUIRED)

include(source_groups.cmake)

add_executable(ogl3_test ${file_root})

target_link_libraries(ogl3_test PRIVATE gl3 glm)
target_compile_features(ogl3_test PRIVATE cxx_auto_type cxx_nullptr)
target_include_directories(ogl3_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(ogl3_test PRIVATE ${SDL2_INCLUDE_DIR})
target_link_libraries(ogl3_test PRIVATE ${SDL2_LIBRARY})

target_include_directories(ogl3_test PRIVATE "${ASSIMP_INCLUDE_DIRS}")
find_library(ASSIMP_LIBRARY "${ASSIMP_LIBRARIES}"
        PATHS ${ASSIMP_LIBRARY_DIRS})
target_link_libraries(ogl3_test PRIVATE ${ASSIMP_LIBRARY})

target_include_directories(ogl3_test PRIVATE ${JANSSON_INCLUDE_DIRS})
target_link_libraries(ogl3_test PRIVATE ${JANSSON_LIBRARIES})
