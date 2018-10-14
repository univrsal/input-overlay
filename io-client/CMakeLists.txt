cmake_minimum_required(VERSION 2.8)
project(io_client)

if(MSVC)
    find_path(SDLNET_INCLUDE_DIRS SDL_ttf.h)
    find_library(SDLNET_LIBRARIES SDL2_net)
else()
    set(CMAKE_C_FLAGS_DEBUG "-D_DEBUG")
    SET(GCC_FLAGS "-fpermissive")
    set(SDLNET_INCLUDE_DIRS "lib/SDL_net/include/")
    #set(SDLNET_LIBRARIES "lib/SDL_net/lib/x64/libSDL2_net-2.0.so.0")
    set(SDLNET_LIBRARIES "${CMAKE_CURRENT_BINARY_DIR}/lib/SDL_net/lib/x64/libSDL2_net-2.0.so.0")
endif()

set(io_client_SOURCES
	src/io_client.cpp)

include_directories(${SDL2_INCLUDE_DIRS}
    ${SDLNET_INCLUDE_DIRS})

add_subdirectory(server)
add_executable(client ${io_client_SOURCES})
target_link_libraries(client ${SDL2_LIBRARIES}
    ${SDLNET_LIBRARIES})
