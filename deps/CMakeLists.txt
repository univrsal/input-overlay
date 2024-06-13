set(COMMON_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/common" CACHE STRING "" FORCE)
set(JSON_11_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/json11" CACHE STRING "" FORCE)
set(JSON_11_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/json11/json11.cpp" CACHE STRING "" FORCE)
set(MONGOOSE_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/mongoose/mongoose.c" CACHE STRING "" FORCE)

set(UIOHOOK_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libuiohook/include" CACHE STRING "" FORCE)
set(MONGOOSE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mongoose" CACHE STRING "" FORCE)

mark_as_advanced(COMMON_HEADERS)
mark_as_advanced(JSON_11_HEADER)
mark_as_advanced(JSON_11_SOURCE)

set(UIOHOOK_ENABLE_TEST OFF CACHE INTERNAL "Internal var")
set(UIOHOOK_ENABLE_SHARED OFF CACHE INTERNAL "Internal var")
set(UIOHOOK_ENABLE_STATIC ON CACHE INTERNAL "Internal var")
set(UIOHOOK_ENABLE_INSTALL OFF CACHE INTERNAL "Internal var")

if (WIN32 OR WIN64)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(SDL2_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/sdl2/bin/x64/SDL2.lib" CACHE STRING "" FORCE)
        set(SDL2_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/sdl2/bin/x64/SDL2.dll" CACHE STRING "" FORCE)
    else()
        set(SDL2_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/sdl2/bin/x86/SDL2.lib" CACHE STRING "" FORCE)
        set(SDL2_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/sdl2/bin/x86/SDL2.dll" CACHE STRING "" FORCE)
    endif()
    set(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/sdl2/include" CACHE STRING "" FORCE)

else ()
    INCLUDE(FindPkgConfig)
    PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
endif ()

# Create static version of libuiohook here
if (WIN32 OR WIN64)
    set(UIOHOOK_SOURCE_DIR "windows")
elseif (APPLE)
    set(UIOHOOK_SOURCE_DIR "darwin")
else()
    set(UIOHOOK_SOURCE_DIR "x11")
endif()

add_library(uiohook_static STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/libuiohook/src/logger.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/libuiohook/src/${UIOHOOK_SOURCE_DIR}/input_helper.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/libuiohook/src/${UIOHOOK_SOURCE_DIR}/input_hook.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/libuiohook/src/${UIOHOOK_SOURCE_DIR}/post_event.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/libuiohook/src/${UIOHOOK_SOURCE_DIR}/system_properties.c"
)

if (WIN32 OR WIN64)
    target_sources(uiohook_static PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/libuiohook/src/${UIOHOOK_SOURCE_DIR}/monitor_helper.c")
endif()

set_target_properties(uiohook_static PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE 1
    VERSION "1.3.0"
    SOVERSION "1"
)

target_include_directories(uiohook_static PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/libuiohook/include")
target_include_directories(uiohook_static PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/libuiohook/src")

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
