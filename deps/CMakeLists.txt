include_directories(common)

add_subdirectory(libuiohook)
add_subdirectory(netlib)
add_subdirectory(libgamepad)

set(NETLIB_ENABLE_TESTS OFF CACHE INTERNAL "Internal var")
set(NETLIB_ENABLE_SHARED OFF CACHE INTERNAL "Internal var")
set(NETLIB_ENABLE_STATIC ON CACHE INTERNAL "Internal var")

set(GAMEPAD_ENABLE_TESTS OFF CACHE INTERNAL "Internal var")
set(GAMEPAD_ENABLE_SHARED OFF CACHE INTERNAL "Internal var")
set(GAMEPAD_ENABLE_STATIC ON CACHE INTERNAL "Internal var")

set(UIOHOOK_ENABLE_TEST OFF CACHE INTERNAL "Internal var")
set(UIOHOOK_ENABLE_SHARED OFF CACHE INTERNAL "Internal var")
set(UIOHOOK_ENABLE_STATIC ON CACHE INTERNAL "Internal var")

