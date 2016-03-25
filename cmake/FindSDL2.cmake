# CMake can't check for SDL2 so we'll just use PkgConfig
find_package(PkgConfig)
include(util)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)

PKG_CONFIG_LIB_RESOLVE(SDL2 SDL2_LIB)
ADD_IMPORTED_LIB(sdl "${SDL2_INCLUDE_DIRS}" "${SDL2_LIB}")
