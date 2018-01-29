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
	add_definitions(-DHAVE_XINPUT=0)
endif()

set(input-overlay_SOURCES
	input-overlay.cpp
	util.cpp
	util.hpp
	input-source.cpp
	input-source.hpp
	input-history.cpp
	input-history.hpp
	layouttype.hpp
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
