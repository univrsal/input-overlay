cmake_minimum_required(VERSION 3.5)
project(io-base VERSION 5.0.0)

option(ENABLE_CLIENT "Wether to build the client (default: ON)" ON)
option(ENABLE_TOOL "Wether to build the config creation tool (default: ON)" ON)
option(ENABLE_PLUGIN "Wether to build the obs plugin (default: ON)" ON)

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

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE DESTINATION ./)
