project(input-overlay)

set(CMAKE_PREFIX_PATH "${QTDIR}")
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(input-overlay_UI
        src/gui/io_settings_dialog.ui)

option(LOCAL_INSTALLATION "Whether to install the obs plugin in the user config directory (default: OFF)" OFF)
option(DEB_INSTALLER "Whether to use the folder structure for a *.deb installer on linux (default: OFF)" OFF)

if (WIN32 OR APPLE)
    include(${CMAKE_SOURCE_DIR}/cmake/FindLibObs.cmake)
endif()

find_package(LibObs REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

string(TIMESTAMP TODAY "%Y.%m.%d %H:%M")
add_definitions(-DBUILD_TIME="${TODAY}")
add_definitions(-DINPUT_OVERLAY_VERSION="${INPUT_OVERLAY_VERSION}")

if (WIN32)
   configure_file(${CMAKE_SOURCE_DIR}/package/installer.iss.in
       ${CMAKE_SOURCE_DIR}/package/installer.iss)
endif()

if (MSVC)
    set(input-overlay_PLATFORM_SOURCES
            src/util/window_helper_win.cpp
            src/hook/uiohook_helper_win.cpp)
    set(OBS_FRONTEND_INCLUDE "${LIBOBS_INCLUDE_DIR}/../UI/")
    set(input-overlay_PLATFORM_DEPS wsock32)
else()
    find_path(OBS_FRONTEND_INCLUDE obs-frontend-api.h)
endif ()

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    add_definitions(-DLINUX=1)
    add_definitions(-DUNIX=1)

    set(input-overlay_PLATFORM_SOURCES
        src/util/window_helper_nix.cpp
        src/hook/uiohook_helper_linux.cpp)
endif ()

if (APPLE)
    # TODO: macOS stuff
endif ()

if (NOT Qt5Widgets_FOUND)
    message(FATAL_ERROR "Failed to find Qt5")
endif ()

set(input-overlay_SOURCES
        src/input_overlay.cpp
        src/sources/input_source.cpp
        src/sources/input_source.hpp
        src/sources/input_source.cpp
        src/hook/uiohook_helper.hpp
        src/hook/gamepad_hook_helper.hpp
        src/hook/gamepad_hook_helper.cpp
        src/gui/io_settings_dialog.cpp
        src/gui/io_settings_dialog.hpp
        src/util/obs_util.cpp
        src/util/obs_util.hpp
        src/util/overlay.cpp
        src/util/overlay.hpp
        src/util/element/element.cpp
        src/util/element/element.hpp
        src/util/element/element_texture.cpp
        src/util/element/element_texture.hpp
        src/util/element/element_button.cpp
        src/util/element/element_button.hpp
        src/util/element/element_mouse_wheel.cpp
        src/util/element/element_mouse_wheel.hpp
        src/util/element/element_trigger.cpp
        src/util/element/element_trigger.hpp
        src/util/element/element_analog_stick.cpp
        src/util/element/element_analog_stick.hpp
        src/util/element/element_gamepad_id.cpp
        src/util/element/element_gamepad_id.hpp
        src/util/element/element_mouse_movement.cpp
        src/util/element/element_mouse_movement.hpp
        src/util/element/element_dpad.cpp
        src/util/element/element_dpad.hpp
        src/util/input_data.hpp
        src/util/input_data.cpp
        src/network/remote_connection.cpp
        src/network/remote_connection.hpp
        src/network/io_server.cpp
        src/network/io_server.hpp
        src/network/io_client.cpp
        src/network/io_client.hpp
        src/network/mg.cpp
        src/network/mg.hpp
        src/util/config.cpp
        src/util/config.hpp
        src/util/input_filter.cpp
        src/util/input_filter.hpp
        src/util/log.h
        src/util/settings.h
        src/util/lang.h src/network/websocket_server.hpp
        src/network/websocket_server.cpp
        ${MONGOOSE_SOURCE})

add_library(input-overlay MODULE
        ${input-overlay_SOURCES}
        ${input-overlay_UI}
        ${input-overlay_PLATFORM_SOURCES})

if (OBS_FRONTEND_INCLUDE)
    include_directories(${OBS_FRONTEND_INCLUDE})
endif()

target_include_directories(
    input-overlay
    PUBLIC
    ${COMMON_HEADERS}
    ${JSON_11_HEADER}
    ${GAMEPAD_INCLUDE_DIR}
    ${UIOHOOK_INCLUDE_DIR}
    ${MONGOOSE_INCLUDE_DIR}
    "${LIBOBS_INCLUDE_DIR}/../UI/obs-frontend-api"
    ${NETLIB_INCLUDE_DIR}
    ${Qt5Core_INCLUDES}
    ${Qt5Widgets_INCLUDES}
)

target_link_libraries(input-overlay
        ${input-overlay_PLATFORM_DEPS}
        Qt5::Core
        Qt5::Widgets
        libobs
        uiohook_static
        netlib_static
        gamepad_static)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_NAME "64bit")
    set(OBS_BUILDDIR_ARCH "build64")
else()
    set(ARCH_NAME "32bit")
    set(OBS_BUILDDIR_ARCH "build32")
endif()

# --- Windows-specific build settings and tasks ---
if(WIN32)
    if(NOT DEFINED OBS_FRONTEND_LIB)
        set(OBS_FRONTEND_LIB "OBS_FRONTEND_LIB-NOTFOUND" CACHE FILEPATH "OBS frontend library")
        message(FATAL_ERROR "Could not find OBS Frontend API\'s library !")
    endif()

    # Enable Multicore Builds and disable FH4 (to not depend on VCRUNTIME140_1.DLL when building with VS2019)
    if (MSVC)
        add_definitions(/MP /d2FH4-)
    endif()

    include_directories(
        "${LIBOBS_INCLUDE_DIR}"
    )

    target_link_libraries(input-overlay
        "${OBS_FRONTEND_LIB}"
    )

    get_filename_component(LibOBS_DIR ${LIBOBS_LIB} DIRECTORY)

    add_custom_command(TARGET input-overlay POST_BUILD
        # Copy to obs-studio dev environment for immediate testing
        COMMAND if $<CONFIG:Debug>==1 (
            "${CMAKE_COMMAND}" -E copy
                "$<TARGET_FILE:input-overlay>"
                "${LibOBS_DIR}/../../rundir/$<CONFIG>/obs-plugins/${ARCH_NAME}")

        COMMAND if $<CONFIG:Debug>==1 (
            "${CMAKE_COMMAND}" -E copy
                "$<TARGET_PDB_FILE:input-overlay>"
                "${LibOBS_DIR}/../../rundir/$<CONFIG>/obs-plugins/${ARCH_NAME}")

        COMMAND if $<CONFIG:Debug>==1 (
            "${CMAKE_COMMAND}" -E make_directory
                "${LibOBS_DIR}/../../rundir/$<CONFIG>/data/obs-plugins/${CMAKE_PROJECT_NAME}")

        COMMAND if $<CONFIG:Debug>==1 (
            "${CMAKE_COMMAND}" -E copy_directory
                "${PROJECT_SOURCE_DIR}/data"
                "${LibOBS_DIR}/../../rundir/$<CONFIG>/data/obs-plugins/input-overlay")
    )
endif()

if(UNIX AND NOT APPLE)
    include(GNUInstallDirs)
    set_target_properties(input-overlay PROPERTIES PREFIX "")
    target_link_libraries(input-overlay obs-frontend-api)

    if (LOCAL_INSTALLATION)
        # Installs into home directory
        install(TARGETS input-overlay
            LIBRARY DESTINATION "$ENV{HOME}/.config/obs-studio/plugins/input-overlay/bin/${ARCH_NAME}")

        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data
            DESTINATION "$ENV{HOME}/.config/obs-studio/plugins/input-overlay/")
    elseif(DEB_INSTALLER)
        # For *.deb installer
        install(TARGETS input-overlay
            LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/obs-plugins")
        install(TARGETS input-overlay
            LIBRARY DESTINATION "/usr/lib/obs-plugins") # Ubuntu apparently doesn't know the correct path

        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data
            DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/obs/obs-plugins/input-overlay")
    else()
        # For *.zip binary
        install(TARGETS input-overlay
            LIBRARY DESTINATION "input-overlay/bin/${ARCH_NAME}")

        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data
            DESTINATION "input-overlay")
    endif()
endif()
