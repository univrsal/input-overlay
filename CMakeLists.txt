project(input-overlay)

if(MSVC)
	set(input-overlay_PLATFORM_DEPS
		w32-pthreads
		XInput)
elseif(UNIX)
	find_package(X11 REQUIRED)
	if(X11_FOUND)
		include_directories(${X11_INCLUDE_DIR})
		set(input-overlay_PLATFORM_DEPS
			${X11_LIBRARIES})
	endif(X11_FOUND)
endif()

set(input-overlay_SOURCES
	input-overlay.cpp
	util.c
	util.h
	input-source.cpp
	input-source.hpp
	input-history.cpp
	input-history.hpp
	layouttype.hpp
	ccl/ccl.cpp
	ccl/ccl.hpp)

add_library(input-overlay MODULE
	${input-overlay_SOURCES})
target_link_libraries(input-overlay
	libobs
	${input-overlay_PLATFORM_DEPS})

install_obs_plugin_with_data(input-overlay data)
