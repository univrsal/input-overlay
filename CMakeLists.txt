cmake_minimum_required(VERSION 3.5)
project(io-base VERSION 5.0.0)
set(INPUT_OVERLAY_VERSION "5.0.0" CACHE STRING "" FORCE)

option(ENABLE_CLIENT "Whether to build the client (default: ON)" ON)
option(ENABLE_PLUGIN "Whether to build the obs plugin (default: ON)" ON)

set(PLUGIN_AUTHOR "univrsal")
set(PLUGIN_GIT input-overlay)
set(LINUX_MAINTAINER_EMAIL "uni@vrsal.de")
set(MACOS_BUNDLEID "de.vrsal.inputoverlay")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

add_subdirectory(deps)

if (ENABLE_CLIENT)
    add_subdirectory(projects/client)
endif()

if (ENABLE_PLUGIN)
    add_subdirectory(projects/plugin)
endif()

# Don't install the license when building the deb installer
if (ENABLE_CLIENT)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/COPYING.txt DESTINATION ./)
endif()
