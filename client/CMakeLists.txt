cmake_minimum_required(VERSION 3.16...3.21)
project(io_client VERSION 5.0.0)

string(TIMESTAMP TODAY "%Y.%m.%d %H:%M")
add_compile_definitions(TIMESTAMP="${TODAY}")

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    add_definitions(-DUNIX=1)
    add_definitions(-DLINUX=1)
    set(client_PLATFORM_DEPS
            pthread)
endif()

if (WIN32 OR WIN64)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(SDL3_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/../deps/sdl3/bin/x64/SDL3.lib")
        set(SDL3_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/../deps/sdl3/bin/x64/SDL3.dll")
        set(client_PLATFORM_DEPS
	        ws2_32
            iphlpapi)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(SDL3_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/../deps/sdl3/bin/x86/SDL3.lib")
        set(SDL3_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/../deps/sdl3/bin/x86/SDL3.dll")
        set(client_PLATFORM_DEPS
            wsock32
            iphlpapi)
    endif()


    set(SDL3_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../deps/sdl3/include")

else ()
    find_package(SDL3 QUIET)
    if(NOT SDL3_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            SDL3
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG release-3.2.22
            GIT_SHALLOW TRUE
        )
        set(SDL_SHARED ON CACHE BOOL "" FORCE)
        set(SDL_STATIC OFF CACHE BOOL "" FORCE)
        set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(SDL3)
    endif()
    set(SDL3_LIBRARIES SDL3::SDL3)
    set(SDL3_INCLUDE_DIRS "")
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
    ${SDL3_LIBRARIES}
    ${client_PLATFORM_DEPS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${SDL3_INCLUDE_DIRS}
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
        ${SDL3_DLLS}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)
    install(FILES ${SDL3_DLLS} DESTINATION ${PROJECT_NAME})

endif()

install(TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_NAME})
