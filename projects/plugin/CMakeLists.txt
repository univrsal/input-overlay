cmake_minimum_required(VERSION 3.5)
project(input-overlay)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(input-overlay_UI
        src/gui/io_settings_dialog.ui)

if (MSVC)
    set(input-overlay_PLATFORM_DEPS
            w32-pthreads)
    set(input-overlay_PLATFORM_SOURCES
            src/util/window_helper_win.cpp)
endif ()

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    add_definitions(-DLINUX=1)
    add_definitions(-DUNIX=1)

    set(input-overlay_PLATFORM_SOURCES
        src/util/window_helper_nix.cpp)
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
        src/input_overlay.cpp
        src/sources/input_source.cpp
        src/sources/input_source.hpp
        src/sources/input_source.cpp
        src/sources/input_history.cpp
        src/sources/input_history.hpp
        src/hook/hook_helper.cpp
        src/hook/hook_helper.hpp
        src/hook/gamepad_hook.hpp
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
        src/util/element/element_data_holder.cpp
        src/util/element/element_data_holder.hpp
        src/util/history/effect.cpp
        src/util/history/effect.hpp
        src/util/history/scale_effect.cpp
        src/util/history/scale_effect.hpp
        src/util/history/translate_effect.cpp
        src/util/history/translate_effect.hpp
        src/util/history/input_entry.cpp
        src/util/history/input_entry.hpp
        src/util/history/input_queue.cpp
        src/util/history/input_queue.hpp
        src/util/history/history_icons.cpp
        src/util/history/history_icons.hpp
        src/util/history/key_names.cpp
        src/util/history/key_names.hpp
        src/util/history/handler.hpp
        src/util/history/icon_handler.cpp
        src/util/history/icon_handler.hpp
        src/util/history/text_handler.cpp
        src/util/history/text_handler.hpp
        src/network/remote_connection.cpp
        src/network/remote_connection.hpp
        src/network/io_server.cpp
        src/network/io_server.hpp
        src/network/io_client.cpp
        src/network/io_client.hpp
        src/util/config.cpp
        src/util/config.hpp
        src/util/input_filter.cpp
        src/util/input_filter.hpp
        src/util/log.h
        src/util/settings.h
        src/util/lang.h)

add_library(input-overlay MODULE
        ${input-overlay_SOURCES}
        ${input-overlay_UI}
        ${input-overlay_PLATFORM_SOURCES})

include_directories(
        SYSTEM "${CMAKE_SOURCE_DIR}/libobs"
        "../../../UI/obs-frontend-api"
        ${Qt5Core_INCLUDES}
        ${Qt5Widgets_INCLUDES})

target_link_libraries(input-overlay
        libobs
        Qt5::Widgets
        obs-frontend-api
        ${input-overlay_PLATFORM_DEPS}
        uiohook}
        netlib
        gamepad)

install_obs_plugin_with_data(input-overlay data)
