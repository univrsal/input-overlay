cmake_minimum_required(VERSION 3.5)
project(input-overlay VERSION 4.9)

find_path(UIOHOOK_INCLUDE_DIR uiohook.h)
find_library(UIOHOOK_LIBRARY uiohook)
find_package(LibObs REQUIRED)

if (MSVC)
    set(input-overlay_PLATFORM_DEPS
            w32-pthreads
            XInput9_1_0)
    add_definitions(-DHAVE_XINPUT=1)
endif ()

if (UNIX)
    add_definitions(-DLINUX=1)
    if (DEBUG)
        add_definitions(-D_DEBUG=1)
    endif ()
endif ()

set(input-overlay_SOURCES
        input-overlay.cpp
        sources/input-source.cpp
        sources/input-source.hpp
        sources/input-history.cpp
        sources/input-history.hpp
        gamepad/linux-gamepad.cpp
        gamepad/linux-gamepad.hpp
        gamepad/windows-gamepad.cpp
        gamepad/windows-gamepad.hpp
        hook/hook-helper.cpp
        hook/hook-helper.hpp
        hook/gamepad-hook.cpp
        hook/gamepad-hook.hpp
        util/util.cpp
        util/util.hpp
        util/layouttype.hpp
        util/layout.hpp
        ccl/ccl.cpp
        ccl/ccl.hpp)

include_directories(${UIOHOOK_INCLUDE_DIR})

add_library(input-overlay MODULE
        ${input-overlay_SOURCES})

target_link_libraries(input-overlay
        libobs
        ${input-overlay_PLATFORM_DEPS}
        ${UIOHOOK_LIBRARY})

if(UNIX AND NOT APPLE)
    # This makes it possible to build this plugin without including it in
	# a full build of OBS Studio.
    include(GNUInstallDirs)

    set_target_properties(input-overlay PROPERTIES PREFIX "")

    file(GLOB locale_files data/locale/*.ini)

    set(CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS
        OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

    install(TARGETS input-overlay LIBRARY
        DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/obs-plugins"
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

    install(FILES ${locale_files}
        DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/obs/obs-plugins/input-overlay/locale")
else()
    # There isn't much need for Windows to build separately.
    install_obs_plugin_with_data(input-overlay data)
endif()
