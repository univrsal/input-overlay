cmake_minimum_required(VERSION 3.16...3.21)

# Change obs-plugintemplate to your plugin's name in a machine-readable format (e.g.:
# obs-myawesomeplugin) and set
project(input-overlay VERSION 5.0.1)
add_library(${CMAKE_PROJECT_NAME} MODULE)
add_subdirectory(deps)

option(LOCAL_INSTALLATION "Whether to install the obs plugin in the user config directory (default: OFF)" OFF)

string(TIMESTAMP TODAY "%Y.%m.%d %H:%M")
add_definitions(-DBUILD_TIME="${TODAY}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/external")

# Replace `Your Name Here` with the name (yours or your organization's) you want to see as the
# author of the plugin (in the plugin's metadata itself and in the installers)
set(PLUGIN_AUTHOR "univrsal")

# Replace `com.example.obs-plugin-template` with a unique Bundle ID for macOS releases (used both in
# the installer and when submitting the installer for notarization)
set(MACOS_BUNDLEID "xyz.vrsal.${CMAKE_PROJECT_NAME}")

# Replace `me@contoso.com` with the maintainer email address you want to put in Linux packages
set(LINUX_MAINTAINER_EMAIL "uni@vrsal.xyz")

option(LOCAL_INSTALLATION "Whether to install for the current user (default: OFF)" OFF)

# Import libobs as main plugin dependency
include(cmake/ObsPluginHelpers.cmake)

# Get the current working branch
execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE)

# Get the latest commit hash
execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    "-DGIT_BRANCH=\"${GIT_BRANCH}\"")
target_compile_definitions(${PROJECT_NAME} PRIVATE
"-DGIT_COMMIT_HASH=\"${GIT_COMMIT_HASH}\"")

# Uncomment these lines if you want to use the OBS Frontend API in your plugin

if (LOCAL_INSTALLATION)

    if (UNIX AND NOT APPLE)
        # Installs into home directory
        add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
            # Copy to obs-studio dev environment for immediate testing
            COMMAND "${CMAKE_COMMAND}" -E make_directory
                    "$ENV{HOME}/.config/obs-studio/plugins/${CMAKE_PROJECT_NAME}/bin/64bit"
            COMMAND "${CMAKE_COMMAND}" -E make_directory
                    "$ENV{HOME}/.config/obs-studio/plugins/${CMAKE_PROJECT_NAME}/data"
            COMMAND "${CMAKE_COMMAND}" -E copy
                    "$<TARGET_FILE:${CMAKE_PROJECT_NAME}>"
                    "$ENV{HOME}/.config/obs-studio/plugins/${CMAKE_PROJECT_NAME}/bin/64bit"
            COMMAND "${CMAKE_COMMAND}" -E copy_directory
                    "${PROJECT_SOURCE_DIR}/data"
                    "$ENV{HOME}/.config/obs-studio/plugins/${CMAKE_PROJECT_NAME}/data"
        )
    endif()
    if (WIN32 OR WIN64)
        # Enable Multicore Builds and disable FH4 (to not depend on VCRUNTIME140_1.DLL when building with VS2019)
        if (MSVC)
            add_definitions(/MP /d2FH4-)
        endif()

        add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
            # Copy to obs-studio dev environment for immediate testing
            COMMAND if $<CONFIG:Debug>==1 (
                "${CMAKE_COMMAND}" -E copy
                    "$<TARGET_FILE:${CMAKE_PROJECT_NAME}>"
                    "${libobs_DIR}/../rundir/$<CONFIG>/obs-plugins/64bit")
            COMMAND if $<CONFIG:Debug>==1 (
                "${CMAKE_COMMAND}" -E copy
                    "$<TARGET_PDB_FILE:${CMAKE_PROJECT_NAME}>"
                    "${libobs_DIR}/../rundir/$<CONFIG>/obs-plugins/64bit")
            COMMAND if $<CONFIG:Debug>==1 (
                "${CMAKE_COMMAND}" -E make_directory
                    "${libobs_DIR}/../rundir/$<CONFIG>/data/obs-plugins/${CMAKE_PROJECT_NAME}")
            COMMAND if $<CONFIG:Debug>==1 (
                "${CMAKE_COMMAND}" -E copy_directory
                    "${PROJECT_SOURCE_DIR}/data"
                    "${libobs_DIR}/../rundir/$<CONFIG>/data/obs-plugins/${CMAKE_PROJECT_NAME}")
        )
    endif()
endif()

if (WIN32 OR WIN64)

        # Enable Multicore Builds and disable FH4 (to not depend on VCRUNTIME140_1.DLL when building with VS2019)
        if (MSVC)
            add_definitions(/MP /d2FH4-)
        endif()
 
        # Copy dependencies
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${SDL2_DLLS}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>)

        install(FILES ${SDL2_DLLS} DESTINATION ${OBS_PLUGIN_DESTINATION})
endif()

find_package(libobs REQUIRED)
find_package(obs-frontend-api REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::obs-frontend-api)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::libobs)
target_link_libraries(${CMAKE_PROJECT_NAME}
        ${input-overlay_PLATFORM_DEPS} PRIVATE
        ${SDL2_LIBRARIES}
        uiohook_static
)

target_include_directories(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${COMMON_HEADERS}
    ${JSON_11_HEADER}
    ${GAMEPAD_INCLUDE_DIR}
    ${UIOHOOK_INCLUDE_DIR}
    ${MONGOOSE_INCLUDE_DIR}
)

if (MSVC)
    set(input-overlay_PLATFORM_SOURCES
            src/util/window_helper_win.cpp
            src/hook/uiohook_helper_win.cpp)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(input-overlay_PLATFORM_DEPS
            ws2_32
            iphlpapi)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(input-overlay_PLATFORM_DEPS
	    wsock32
            iphlpapi)
    endif()

elseif ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    add_definitions(-DLINUX=1)
    add_definitions(-DUNIX=1)

    set(input-overlay_PLATFORM_SOURCES
        src/util/window_helper_nix.cpp
        src/hook/uiohook_helper_linux.cpp)
endif()

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE src)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        src/input_overlay.cpp
        src/sources/input_source.cpp
        src/sources/input_source.hpp
        src/sources/input_source.cpp
        src/hook/sdl_gamepad.hpp
        src/hook/sdl_gamepad.cpp
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
        src/network/remote_connection.cpp
        src/network/remote_connection.hpp
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
        src/gui/io_settings_dialog.ui
        ${input-overlay_PLATFORM_SOURCES}
        ${COMMON_HEADERS}/input_data.cpp
        ${MONGOOSE_SOURCE})


# Uncomment those lines if you want to use Qt in your plugin
find_qt(COMPONENTS Widgets Core)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt::Core Qt::Widgets)
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES AUTOMOC ON AUTOUIC ON AUTORCC ON)

configure_file(src/plugin-macros.h.in ${CMAKE_SOURCE_DIR}/src/plugin-macros.generated.h)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE src/plugin-macros.generated.h)

# /!\ TAKE NOTE: No need to edit things past this point /!\

# --- Platform-independent build settings ---

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/projects/plugin/src)


# --- End of section ---

# --- Windows-specific build settings and tasks ---
if(OS_WINDOWS)
  configure_file(cmake/bundle/windows/installer-Windows.iss.in
                 ${CMAKE_BINARY_DIR}/installer-Windows.generated.iss)

  if(MSVC)
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /W4)
  endif()
  # --- End of section ---

  # -- macOS specific build settings and tasks --
elseif(OS_MACOS)
  configure_file(cmake/bundle/macos/installer-macos.pkgproj.in
                 ${CMAKE_BINARY_DIR}/installer-macos.generated.pkgproj)

  set(MACOSX_PLUGIN_GUI_IDENTIFIER "${MACOS_BUNDLEID}")
  set(MACOSX_PLUGIN_BUNDLE_VERSION "${CMAKE_PROJECT_VERSION}")
  set(MACOSX_PLUGIN_SHORT_VERSION_STRING "1")

  target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall)
  # --- End of section ---

  # --- Linux-specific build settings and tasks ---
else()
  target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall)
endif()
# --- End of section ---

setup_plugin_target(${CMAKE_PROJECT_NAME})
