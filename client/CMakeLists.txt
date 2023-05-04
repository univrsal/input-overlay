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

if (WIN32 OR WIN64)
    set(UIOHOOK_SOURCE_DIR "windows")
elseif (APPLE)
    set(UIOHOOK_SOURCE_DIR "darwin")
else()
    set(UIOHOOK_SOURCE_DIR "x11")
endif()

add_library(uiohook_static STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../deps/libuiohook/src/logger.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../deps/libuiohook/src/${UIOHOOK_SOURCE_DIR}/input_helper.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../deps/libuiohook/src/${UIOHOOK_SOURCE_DIR}/input_hook.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../deps/libuiohook/src/${UIOHOOK_SOURCE_DIR}/post_event.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../deps/libuiohook/src/${UIOHOOK_SOURCE_DIR}/system_properties.c"
)

if (WIN32 OR WIN64)
    target_sources(uiohook_static PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../deps/libuiohook/src/${UIOHOOK_SOURCE_DIR}/monitor_helper.c")
endif()

set_target_properties(uiohook_static PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE 1
    VERSION "1.2.1"
    SOVERSION "1"
)

target_include_directories(uiohook_static PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../deps/libuiohook/include")
target_include_directories(uiohook_static PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../deps/libuiohook/src")

if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(X11 REQUIRED x11)
    target_include_directories(uiohook_static PRIVATE "${X11_INCLUDE_DIRS}")
    target_link_libraries(uiohook_static "${X11_LDFLAGS}")

    pkg_check_modules(XTST REQUIRED xtst)
    target_include_directories(uiohook_static PRIVATE "${XTST_INCLUDE_DIRS}")
    target_link_libraries(uiohook_static "${XTST_LDFLAGS}")

    include(CheckLibraryExists)
    check_library_exists(Xtst XRecordQueryVersion "" HAVE_XRECORD)

    include(CheckIncludeFile)
    check_include_file(X11/extensions/record.h HAVE_RECORD_H "-include X11/Xlib.h")

    option(USE_XKB_COMMON "X Keyboard Common Extension (default: ON)" ON)
    if(USE_XKB_COMMON)
        pkg_check_modules(XKB_COMMON REQUIRED xkbcommon-x11)
        add_compile_definitions(uiohook_static PRIVATE USE_XKB_COMMON)
        target_include_directories(uiohook_static PRIVATE "${XKB_COMMON_INCLUDE_DIRS}")
        target_link_libraries(uiohook_static "${XKB_COMMON_LDFLAGS}")

        pkg_check_modules(X11_XCB REQUIRED x11-xcb)
        target_include_directories(uiohook_static PRIVATE "${X11_XCB_INCLUDE_DIRS}")
        target_link_libraries(uiohook_static "${X11_XCB_LDFLAGS}")
    endif()

    option(USE_XKB_FILE "X Keyboard File Extension (default: ON)" ON)
    if(USE_XKB_FILE)
        pkg_check_modules(XKB_FILE REQUIRED xkbfile)
        add_compile_definitions(uiohook_static PRIVATE USE_XKB_FILE)
        target_include_directories(uiohook_static PRIVATE "${XKB_FILE_INCLUDE_DIRS}")
        target_link_libraries(uiohook_static "${XKB_FILE_LDFLAGS}")
    endif()

    option(USE_XT "X Toolkit Extension (default: ON)" ON)
    if(USE_XT)
        pkg_check_modules(XT REQUIRED xt)
        add_compile_definitions(uiohook_static PRIVATE USE_XT)
        target_include_directories(uiohook_static PRIVATE "${XT_INCLUDE_DIRS}")
        target_link_libraries(uiohook_static "${XT_LDFLAGS}")
    endif()


    option(USE_XF86MISC "XFree86-Misc X Extension (default: OFF)" OFF)
    if(USE_XF86MISC)
        pkg_check_modules(XF86MISC REQUIRED Xxf86misc)
        add_compile_definitions(uiohook_static PRIVATE USE_XF86MISC)
        target_include_directories(uiohook_static PRIVATE "${XF86MISC_INCLUDE_DIRS}")
        target_link_libraries(uiohook_static "${XF86MISC_LDFLAGS}")
    endif()

    option(USE_XRANDR "XRandR Extension (default: OFF)" OFF)
    if(USE_XRANDR)
        pkg_check_modules(XRANDR REQUIRED xrandr)
        add_compile_definitions(uiohook_static PRIVATE USE_XRANDR)
        target_include_directories(uiohook_static PRIVATE "${XRANDR_INCLUDE_DIRS}")
        target_link_libraries(uiohook_static "${XRANDR_LDFLAGS}")
    endif()

    option(USE_XINERAMA "Xinerama Extension (default: ON)" ON)
    if(USE_XINERAMA)
        pkg_check_modules(XINERAMA REQUIRED xinerama)
        add_compile_definitions(uiohook_static PRIVATE USE_XINERAMA)
        target_include_directories(uiohook_static PRIVATE "${XINERAMA_INCLUDE_DIRS}")
        target_link_libraries(uiohook_static "${XINERAMA_LDFLAGS}")
    endif()

    option(USE_XRECORD_ASYNC "XRecord Asynchronous API (default: OFF)" OFF)
    if(USE_XRECORD_ASYNC)
        add_compile_definitions(uiohook_static PRIVATE USE_XRECORD_ASYNC)
    endif()

    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        option(USE_EVDEV "Generic Linux input driver (default: ON)" ON)
        if(USE_EVDEV)
            add_compile_definitions(uiohook_static PRIVATE USE_EVDEV)
        endif()
    endif()
elseif(APPLE)
    set(CMAKE_MACOSX_RPATH 1)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.5")

    find_package(Threads REQUIRED)
    target_link_libraries(uiohook_static "${CMAKE_THREAD_LIBS_INIT}")

    find_library(CARBON Carbon REQUIRED)
    target_include_directories(uiohook_static PRIVATE "${CARBON}")
    target_link_libraries(uiohook_static "${CARBON}")

    option(USE_APPLICATION_SERVICES "ApplicationServices framework (default: ON)" ON)
    if(USE_APPLICATION_SERVICES)
        find_library(APPLICATION_SERVICES ApplicationServices REQUIRED)
        add_compile_definitions(USE_APPLICATION_SERVICES)
        target_include_directories(uiohook_static PRIVATE "${APPLICATION_SERVICES}")
        target_link_libraries(uiohook_static "${APPLICATION_SERVICES}")
    endif()

    option(USE_IOKIT "IOKit framework (default: ON)" ON)
    if(USE_IOKIT)
        find_library(IOKIT IOKit REQUIRED)
        add_compile_definitions(USE_IOKIT)
        target_include_directories(uiohook_static PRIVATE "${IOKIT}")
        target_link_libraries(uiohook_static "${IOKIT}")
    endif()

    option(USE_APPKIT "AppKit framework (default: ON)" ON)
    if(USE_APPKIT)
        find_library(APPKIT AppKit REQUIRED)
        add_compile_definitions(USE_APPKIT)
        target_include_directories(uiohook_static PRIVATE "${APPKIT}")
        target_link_libraries(uiohook_static "${APPKIT}")
    endif()
elseif(WIN32)
    target_link_libraries(uiohook_static Advapi32)
endif()

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

target_link_libraries(${PROJECT_NAME}
    uiohook_static
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
