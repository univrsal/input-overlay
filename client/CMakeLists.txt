cmake_minimum_required(VERSION 3.16...3.21)
project(io_client VERSION 5.0.0)

string(TIMESTAMP TODAY "%Y.%m.%d %H:%M")
add_compile_definitions(TIMESTAMP="${TODAY}")

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    add_definitions(-DUNIX=1)
    add_definitions(-DLINUX=1)
    set(client_PLATFORM_DEPS
            pthread)
    INCLUDE(FindPkgConfig)
    PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
endif()

if (WIN32 OR WIN64)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(SDL2_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/../deps/sdl2/bin/x64/SDL2.lib")
        set(SDL2_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/../deps/sdl2/bin/x64/SDL2.dll")
        set(client_PLATFORM_DEPS
	        ws2_32
            iphlpapi)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(SDL2_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/../deps/sdl2/bin/x86/SDL2.lib")
        set(SDL2_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/../deps/sdl2/bin/x86/SDL2.dll")
        set(client_PLATFORM_DEPS
            wsock32
            iphlpapi)
    endif()


    set(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../deps/sdl2/include")

endif ()

set(io_client_SOURCES
    ../deps/mongoose/mongoose.c
    ../deps/mongoose/mongoose.h
    ../deps/json11/json11.cpp

    src/argparse.c
    src/argparse.h
    src/io_client.cpp
    src/client_util.cpp
    src/client_util.hpp
    src/network_helper.cpp
    src/network_helper.hpp
    src/gamepad_helper.cpp
    src/gamepad_helper.hpp
    src/uiohook_helper.cpp
    src/uiohook_helper.hpp)

add_executable(${PROJECT_NAME} ${io_client_SOURCES})

add_subdirectory(../deps/libuiohook ${CMAKE_CURRENT_BINARY_DIR}/libuiohook)

target_link_libraries(${PROJECT_NAME}
    uiohook
    ${SDL2_LIBRARIES}
    ${client_PLATFORM_DEPS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ../deps/libuiohook/include
    ../deps/common
    ../deps/json11
    ../deps/mongoose
)

if (WIN32 OR WIN64)
    if (MSVC)
        add_definitions(/MP /d2FH4-)
    endif()

    # Copy dependencies
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_DLLS}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)
    install(FILES ${SDL2_DLLS} DESTINATION ${PROJECT_NAME})

endif()

install(TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_NAME})
