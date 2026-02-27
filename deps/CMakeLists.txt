set(COMMON_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/common" CACHE STRING "" FORCE)
set(JSON_11_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/json11" CACHE STRING "" FORCE)
set(JSON_11_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/json11/json11.cpp" CACHE STRING "" FORCE)
set(MONGOOSE_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/mongoose/mongoose.c" CACHE STRING "" FORCE)

set(MONGOOSE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mongoose" CACHE STRING "" FORCE)

mark_as_advanced(COMMON_HEADERS)
mark_as_advanced(JSON_11_HEADER)
mark_as_advanced(JSON_11_SOURCE)

if (WIN32 OR WIN64)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(SDL3_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/sdl3/bin/x64/SDL3.lib" CACHE STRING "" FORCE)
        set(SDL3_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/sdl3/bin/x64/SDL3.dll" CACHE STRING "" FORCE)
    else()
        set(SDL3_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/sdl3/bin/x86/SDL3.lib" CACHE STRING "" FORCE)
        set(SDL3_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/sdl3/bin/x86/SDL3.dll" CACHE STRING "" FORCE)
    endif()
    set(SDL3_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/sdl3/include" CACHE STRING "" FORCE)

else ()
    # On macOS, always build from source to support universal binaries (arm64+x86_64).
    # Homebrew SDL3 is typically single-architecture only.
    if(NOT APPLE)
        find_package(SDL3 QUIET)
    endif()
    if(NOT SDL3_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            SDL3
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG release-3.2.22
            GIT_SHALLOW TRUE
        )
        set(SDL_SHARED ON CACHE BOOL "" FORCE)
        set(SDL_STATIC OFF CACHE BOOL "" FORCE)
        set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(SDL3)
    endif()
    set(SDL3_LIBRARIES SDL3::SDL3 CACHE STRING "" FORCE)
    set(SDL3_INCLUDE_DIRS "" CACHE STRING "" FORCE)
endif ()

add_subdirectory(libuiohook)
if(APPLE)
  target_compile_options(uiohook PRIVATE -Wno-error=return-type)
endif()
