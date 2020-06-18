project(io_client)

find_path(UIOHOOK_INCLUDE_DIR uiohook.h)
find_library(UIOHOOK_LIBRARY uiohook)

if(MSVC)
    set(client_PLATFORM_DEPS)
    find_path(NETLIB_INCLUDE_DIR netlib.h)
    find_library(NETLIB_LIBRARY netlib)
    set(io_client_PLATFORM_SOURCES
        src/gamepad_win.cpp)
endif()

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    add_definitions(-DUNIX=1)
    add_definitions(-DLINUX=1)
    set(client_PLATFORM_DEPS
            pthread)
    set(NETLIB_INCLUDE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/../netlib/include)
    set(NETLIB_LIBRARY
        ${CMAKE_CURRENT_SOURCE_DIR}/../netlib/bin/linux64/libnetlib.so)
    set(io_client_PLATFORM_SOURCES
        src/gamepad_linux.cpp)
endif()


set(io_client_SOURCES
    ${io_client_PLATFORM_SOURCES}
    src/io_client.cpp
    src/util.cpp
    src/util.hpp
    src/network.cpp
    src/network.hpp
    src/uiohook.cpp
    src/uiohook.hpp
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
