CMAKE_MINIMUM_REQUIRED(VERSION 3.2)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(GL3Test)

if (NOT MSVC)
    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-Wall -Wno-unused-function")
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -Wno-unused-function")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_subdirectory(lib)

add_subdirectory(src)
