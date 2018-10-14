cmake_minimum_required(VERSION 2.8)
project(io_server)

if(MSVC)
    find_path(SDLNET_INCLUDE_DIRS SDL_ttf.h)
    find_library(SDLNET_LIBRARIES SDL2_net)
else()
    set(CMAKE_C_FLAGS_DEBUG "-D_DEBUG")
    SET(GCC_FLAGS "-fpermissive")

    set(SDLNET_INCLUDE_DIRS "../lib/SDL_net/include/")
    set(SDLNET_LIBRARIES "${CMAKE_CURRENT_BINARY_DIR}/../lib/SDL_net/lib/x64/libSDL2_net-2.0.so.0.0.1")
endif()

set(io_server_SOURCES
	src/io_server.cpp)

include_directories(${SDL2_INCLUDE_DIRS}
    ${SDLNET_INCLUDE_DIRS})

add_executable(server ${io_server_SOURCES})
target_link_libraries(server  ${SDL2_LIBRARIES}
    ${SDLNET_LIBRARIES})
