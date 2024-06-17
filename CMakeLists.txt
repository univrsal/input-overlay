cmake_minimum_required(VERSION 3.16...3.26)

set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/bootstrap.cmake" NO_POLICY_SCOPE)

project(${_name} VERSION ${_version})

option(ENABLE_FRONTEND_API "Use obs-frontend-api for UI functionality" ON)
option(ENABLE_QT "Use Qt functionality" ON)

include(compilerconfig)
include(defaults)
include(helpers)

add_library(${CMAKE_PROJECT_NAME} MODULE)
add_subdirectory(deps)

# SDL2 headers clash with obs simd stuff for some reason
if (NOT DISABLE_NATIVE_ARCH)
    set(CMAKE_CXX_FLAGS "-march=native")
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
        
        install(FILES ${SDL2_DLLS} DESTINATION
            CONFIGURATIONS RelWithDebInfo Debug Release
            DESTINATION obs-plugins/64bit
        )
endif()

string(TIMESTAMP TODAY "%Y.%m.%d %H:%M")

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

target_compile_definitions(${PROJECT_NAME} PUBLIC
    GIT_BRANCH="${GIT_BRANCH}"
    GIT_COMMIT_HASH="${GIT_COMMIT_HASH}"
    PLUGIN_VERSION="${_version}"
    BUILD_TIME="${TODAY}"
)

find_package(libobs REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::libobs)


find_package(obs-frontend-api REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::obs-frontend-api)


find_package(Qt6 COMPONENTS Widgets Core)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets)
target_compile_options(
${CMAKE_PROJECT_NAME} PRIVATE $<$<C_COMPILER_ID:Clang,AppleClang>:-Wno-quoted-include-in-framework-header
                                -Wno-comma>)
set_target_properties(
${CMAKE_PROJECT_NAME}
PROPERTIES AUTOMOC ON
            AUTOUIC ON)

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
endif()

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
        ${input-overlay_PLATFORM_DEPS}
        ${SDL2_LIBRARIES}
        uiohook_static
)

target_include_directories(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${COMMON_HEADERS}
    ${JSON_11_HEADER}
    ${UIOHOOK_INCLUDE_DIR}
    ${MONGOOSE_INCLUDE_DIR}
)

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

set_target_properties_plugin(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${_name})
configure_file(src/plugin-macros.h.in ${CMAKE_SOURCE_DIR}/src/plugin-macros.generated.h)
