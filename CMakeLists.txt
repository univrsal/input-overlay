cmake_minimum_required(VERSION 3.5)
project(io-base VERSION 5.0.0)
set(INPUT_OVERLAY_VERSION "5.0.0" CACHE STRING "" FORCE)

option(ENABLE_CLIENT "Wether to build the client (default: ON)" ON)
option(ENABLE_TOOL "Wether to build the config creation tool (default: ON)" ON)
option(ENABLE_PLUGIN "Wether to build the obs plugin (default: ON)" ON)

set(PLUGIN_AUTHOR "univrsal")
set(PLUGIN_GIT input-overlay)
set(LINUX_MAINTAINER_EMAIL "uni@vrsal.cf")
set(MACOS_BUNDLEID "cf.vrsal.inputoverlay")

add_subdirectory(deps)

if (ENABLE_CLIENT)
    add_subdirectory(projects/client)
endif()

if (ENABLE_TOOL)
    add_subdirectory(projects/cct)
endif()

if (ENABLE_PLUGIN)
    add_subdirectory(projects/plugin)
endif()

# Don't install the license when building the deb installer
if (ENABLE_CLIENT OR ENABLE_TOOL)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/COPYING.txt DESTINATION ./)
endif()
