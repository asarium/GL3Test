
add_library(glad_3 STATIC
        "release/include/glad/glad.h"
        "release/include/KHR/khrplatform.h"
        "release/src/glad.c")
target_include_directories(glad_3 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/release/include")
set_target_properties(glad_3 PROPERTIES FOLDER "3rdparty")
if (MSVC)
    # Suppress some warnings
    target_compile_options(glad_3 PRIVATE "/wd4090" "/wd4055" "/wd4054")
endif()

add_library(glad_3_debug STATIC
        "debug/include/glad/glad.h"
        "debug/include/KHR/khrplatform.h"
        "debug/src/glad.c")
target_include_directories(glad_3_debug PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/debug/include")
set_target_properties(glad_3_debug PROPERTIES FOLDER "3rdparty")
if (MSVC)
    # Suppress some warnings
    target_compile_options(glad_3_debug PRIVATE "/wd4090" "/wd4055" "/wd4054" "/wd4100")
endif()


add_library(gl3 INTERFACE)
target_link_libraries(gl3 INTERFACE "$<$<CONFIG:Release>:glad_3>" "$<$<NOT:$<CONFIG:Release>>:glad_3_debug>")
