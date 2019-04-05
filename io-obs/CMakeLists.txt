project(input-overlay)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

option(ENABLE_STATIC_NETLIB "Use packaged netlib version" ON)
option(ENABLE_STATIC_UIOHOOK "Use packged uiohook version" ON)

if (MSVC)
    set(input-overlay_PLATFORM_DEPS
            w32-pthreads)

    find_path(NETLIB_INCLUDE_DIR netlib.h)
    find_library(NETLIB_LIBRARY netlib)

    find_path(UIOHOOK_INCLUDE_DIR uiohook.h)
    find_library(UIOHOOK_LIBRARY uiohook)
endif ()

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    add_definitions(-DLINUX=1)
    add_definitions(-DUNIX=1)

    set(input-overlay_PLATFORM_SOURCES
            util/window_helper.hpp util/window_helper_nix.cpp hook/gamepad_binding.cpp hook/gamepad_binding.hpp)

    if (ENABLE_STATIC_NETLIB)
        message("-- [input-overlay] Using precompiled netlib")
        set(NETLIB_INCLUDE_DIR
                ${CMAKE_CURRENT_SOURCE_DIR}/../netlib/include)
        set(NETLIB_LIBRARY
                ${CMAKE_CURRENT_SOURCE_DIR}/../netlib/bin/linux64/libnetlib.so)
    else ()
        find_path(NETLIB_INCLUDE_DIR netlib.h)
        find_library(NETLIB_LIBRARY netlib)
    endif ()

    if (ENABLE_STATIC_UIOHOOK)
        message("-- [input-overlay] Using precompiled uiohook")
        set(UIOHOOK_INCLUDE_DIR
                ${CMAKE_CURRENT_SOURCE_DIR}/../libuiohook/include)
        set(UIOHOOK_LIBRARY
                ${CMAKE_CURRENT_SOURCE_DIR}/../libuiohook/bin/linux64/libuiohook.so.0)
    else ()
        find_path(UIOHOOK_INCLUDE_DIR uiohook.h)
        find_library(UIOHOOK_LIBRARY uiohook)
    endif ()

endif ()

if (APPLE)
    # TODO: macOS stuff
endif ()

macro(use_cxx14)
    if (CMAKE_VERSION VERSION_LESS "3.1")
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")
        endif ()
    else ()
        set(CMAKE_CXX_STANDARD 14)
    endif ()
endmacro(use_cxx14)

use_cxx14()

find_package(Qt5Widgets REQUIRED)

if (NOT Qt5Widgets_FOUND)
    message(FATAL_ERROR "Failed to find Qt5")
endif ()

set(input-overlay_SOURCES
        input_overlay.cpp
        sources/input_source.cpp
        sources/input_source.hpp
        sources/input_source.cpp
        sources/input_history.cpp
        sources/input_history.hpp
        hook/hook_helper.cpp
        hook/hook_helper.hpp
        hook/gamepad_hook.cpp
        hook/gamepad_hook.hpp
        hook/xinput_fix.cpp
        hook/xinput_fix.hpp
        util/util.cpp
        util/util.hpp
        util/overlay.cpp
        util/overlay.hpp
        util/layout_constants.hpp
        util/element/element.cpp
        util/element/element.hpp
        util/element/element_texture.cpp
        util/element/element_texture.hpp
        util/element/element_button.cpp
        util/element/element_button.hpp
        util/element/element_mouse_wheel.cpp
        util/element/element_mouse_wheel.hpp
        util/element/element_trigger.cpp
        util/element/element_trigger.hpp
        util/element/element_analog_stick.cpp
        util/element/element_analog_stick.hpp
        util/element/element_gamepad_id.cpp
        util/element/element_gamepad_id.hpp
        util/element/element_mouse_movement.cpp
        util/element/element_mouse_movement.hpp
        util/element/element_dpad.cpp
        util/element/element_dpad.hpp
        util/element/element_data_holder.cpp
        util/element/element_data_holder.hpp
        util/history/effect.cpp
        util/history/effect.hpp
        util/history/scale_effect.cpp
        util/history/scale_effect.hpp
        util/history/translate_effect.cpp
        util/history/translate_effect.hpp
        util/history/input_entry.cpp
        util/history/input_entry.hpp
        util/history/input_queue.cpp
        util/history/input_queue.hpp
        util/history/history_icons.cpp
        util/history/history_icons.hpp
        util/history/key_names.cpp
        util/history/key_names.hpp
        util/history/handler.hpp
        util/history/icon_handler.cpp
        util/history/icon_handler.hpp
        util/history/text_handler.cpp
        util/history/text_handler.hpp
        gui/io_settings_dialog.hpp
        gui/io_settings_dialog.cpp
        network/remote_connection.cpp
        network/remote_connection.hpp
        network/io_server.cpp
        network/io_server.hpp
        network/io_client.cpp
        network/io_client.hpp
        ../ccl/ccl.cpp
        ../ccl/ccl.hpp)

set(io_settings_UI
        gui/io_settings_dialog.ui)

qt5_wrap_ui(frontend-tools_UI_HEADERS
        ${io_settings_UI})

add_library(input-overlay MODULE
        ${io_settings_UI_HEADERS}
        ${input-overlay_SOURCES}
        ${input-overlay_PLATFORM_SOURCES})

include_directories(
        ${UIOHOOK_INCLUDE_DIR}
        ${NETLIB_INCLUDE_DIR}
        SYSTEM "${CMAKE_SOURCE_DIR}/libobs"
        "../../../UI/obs-frontend-api"
        ${Qt5Core_INCLUDES}
        ${Qt5Widgets_INCLUDES})

target_link_libraries(input-overlay
        libobs
        Qt5::Widgets
        obs-frontend-api
        ${input-overlay_PLATFORM_DEPS}
        ${UIOHOOK_LIBRARY}
        ${NETLIB_LIBRARY})

install_obs_plugin_with_data(input-overlay data)
