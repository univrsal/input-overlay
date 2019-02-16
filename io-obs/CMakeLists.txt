project(input-overlay)

find_path(UIOHOOK_INCLUDE_DIR uiohook.h)
find_library(UIOHOOK_LIBRARY uiohook)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

if(MSVC)
    set(input-overlay_PLATFORM_DEPS
        w32-pthreads)

    find_path(NETLIB_INCLUDE_DIR netlib.h)
    find_library(NETLIB_LIBRARY netlib)
endif()

if(UNIX)
    add_definitions(-DLINUX_INPUT=1)
    add_definitions(-DUNIX=1)
    set(NETLIB_INCLUDE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/../netlib/include)
    set(NETLIB_LIBRARY
        ${CMAKE_CURRENT_SOURCE_DIR}/../netlib/bin/linux64/libnetlib.so)
endif()

macro(use_cxx14)
    if (CMAKE_VERSION VERSION_LESS "3.1")
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")
        endif ()
    else ()
        set (CMAKE_CXX_STANDARD 14)
    endif ()
endmacro(use_cxx14)

use_cxx14()

#find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

if(NOT Qt5Widgets_FOUND)
    message(FATAL_ERROR "Failed to find Qt5")
endif()

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
	${input-overlay_SOURCES})

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
