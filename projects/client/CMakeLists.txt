project(io_client)

string(TIMESTAMP TODAY "%Y.%m.%d %H:%M")
add_compile_definitions(TIMESTAMP="${TODAY}")

find_path(UIOHOOK_INCLUDE_DIR uiohook.h)
find_library(UIOHOOK_LIBRARY uiohook)

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    add_definitions(-DUNIX=1)
    add_definitions(-DLINUX=1)
    set(client_PLATFORM_DEPS
            pthread)
endif()


set(io_client_SOURCES
    src/io_client.cpp
    src/client_util.cpp
    src/client_util.hpp
    src/network.cpp
    src/network.hpp
    src/gamepad_helper.cpp
    src/gamepad_helper.hpp
    src/uiohook_helper.cpp
    src/uiohook_helper.hpp)

add_executable(client ${io_client_SOURCES})

target_link_libraries(client
    uiohook_static
    netlib_static
    gamepad_static
    ${client_PLATFORM_DEPS})

include_directories(
    ${COMMON_HEADERS}
    ${JSON_11_HEADER}
    ${GAMEPAD_INCLUDE_DIR}
    ${UIOHOOK_INCLUDE_DIR}
    ${NETLIB_INCLUDE_DIR}
    )
