cmake_minimum_required(VERSION 2.8)
project(io_server)

if(MSVC)
    find_path(SDLNET_INCLUDE_DIRS SDL_ttf.h)
    find_library(SDLNET_LIBRARIES SDL2_net)
else()
    set(CMAKE_C_FLAGS_DEBUG "-D_DEBUG")
    SET(GCC_FLAGS "-fpermissive")
    INCLUDE(FindPkgConfig)

    PKG_SEARCH_MODULE(SDL2NET REQUIRED)
endif()

set(io_server_SOURCES
	src/io_server.cpp)

include_directories(${SDL2_INCLUDE_DIRS}
    ${SDLNET_INCLUDE_DIRS})

add_executable(server ${io_server_SOURCES})
target_link_libraries(server  ${SDL2_LIBRARIES}
    ${SDLNET_LIBRARIES})