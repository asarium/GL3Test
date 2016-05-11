
add_library(gli INTERFACE)
target_include_directories(gli INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(gli INTERFACE cxx_rvalue_references)
target_link_libraries(gli INTERFACE glm)
