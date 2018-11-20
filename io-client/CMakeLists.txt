cmake_minimum_required(VERSION 2.8)
project(io_client)

find_path(UIOHOOK_INCLUDE_DIR uiohook.h)
find_library(UIOHOOK_LIBRARY uiohook)

if(MSVC)
    set(client_PLATFORM_DEPS)
    find_path(NETLIB_INCLUDE_DIR netlib.h)
    find_library(NETLIB_LIBRARY netlib)
endif()

if(UNIX)
    add_definitions(-DUNIX=1)
    set(client_PLATFORM_DEPS
            pthread)
    set(NETLIB_INCLUDE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/../netlib/include)
    set(NETLIB_LIBRARY
        ${CMAKE_CURRENT_SOURCE_DIR}/../netlib/bin/linux64/libnetlib.so)
endif()


set(io_client_SOURCES
    src/io_client.cpp
    src/util.cpp
    src/util.hpp
    src/network.cpp
    src/network.hpp
    src/uiohook.cpp
    src/uiohook.hpp
    src/gamepad.cpp
    src/gamepad.hpp
    src/xinput_fix.cpp
    src/xinput_fix.hpp
    src/gamepad_state.cpp
    src/gamepad_state.hpp)

include_directories(${NETLIB_INCLUDE_DIR}
    ${UIOHOOK_INCLUDE_DIR})

add_executable(client ${io_client_SOURCES})
target_link_libraries(client ${UIOHOOK_LIBRARY}
    ${NETLIB_LIBRARY}
    ${client_PLATFORM_DEPS})
