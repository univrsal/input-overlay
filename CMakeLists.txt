project(input-overlay)

find_path(UIOHOOK_INCLUDE_DIR uiohook.h)
find_library(UIOHOOK_LIBRARY uiohook)

if(MSVC)
	set(input-overlay_PLATFORM_DEPS
		w32-pthreads
		XInput)
	add_definitions(-DHAVE_XINPUT=1)
endif()

if(UNIX)
	add_definitions(-DLINUX_INPUT=1)
endif()

set(input-overlay_SOURCES
	input-overlay.cpp
	sources/input-source.cpp
	sources/input-source.hpp
	sources/input-history.cpp
	sources/input-history.hpp
	gamepad/linux-gamepad.cpp
	gamepad/linux-gamepad.hpp
	gamepad/windows-gamepad.cpp
	gamepad/windows-gamepad.hpp
	hook/hook-helper.cpp
	hook/hook-helper.hpp
	hook/gamepad-hook.cpp
	hook/gamepad-hook.hpp
	util/util.cpp
	util/util.hpp
	util/layouttype.hpp
	util/layout.hpp
	ccl/ccl.cpp
	ccl/ccl.hpp)

include_directories(${UIOHOOK_INCLUDE_DIR})

add_library(input-overlay MODULE
	${input-overlay_SOURCES})

target_link_libraries(input-overlay
	libobs
	${input-overlay_PLATFORM_DEPS}
	${UIOHOOK_LIBRARY})

install_obs_plugin_with_data(input-overlay data)
