cmake_minimum_required(VERSION 3.5)
project(input-overlay VERSION 4.9)

find_path(UIOHOOK_INCLUDE_DIR uiohook.h)
find_library(UIOHOOK_LIBRARY uiohook)
find_package(LibObs REQUIRED)

if (MSVC)
    set(input-overlay_PLATFORM_DEPS
            w32-pthreads
            XInput9_1_0)
    add_definitions(-DHAVE_XINPUT=1)
endif ()

if (UNIX)
    add_definitions(-DLINUX=1)
    if (DEBUG)
        add_definitions(-D_DEBUG=1)
    endif ()
endif ()

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

# --- Windows-specific build settings and tasks ---
if(WIN32)
    if(MSVC)
        # Enable Multicore Builds and disable FH4 (to not depend on VCRUNTIME140_1.DLL)
	    add_definitions(/MP /d2FH4-)
    endif()

	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(ARCH_NAME "64bit")
		set(OBS_BUILDDIR_ARCH "build64")
	else()
		set(ARCH_NAME "32bit")
		set(OBS_BUILDDIR_ARCH "build32")
	endif()

	# --- Release package helper ---
	# The "release" folder has a structure similar OBS' one on Windows
	set(RELEASE_DIR "${PROJECT_SOURCE_DIR}/release")

	add_custom_command(TARGET input-overlay POST_BUILD
		# If config is Release, package release files
		COMMAND if $<CONFIG:Release>==1 (
			"${CMAKE_COMMAND}" -E make_directory
			"${RELEASE_DIR}/data/obs-plugins/input-overlay"
			"${RELEASE_DIR}/obs-plugins/${ARCH_NAME}")

		COMMAND if $<CONFIG:Release>==1 ("${CMAKE_COMMAND}" -E copy_directory
			"${PROJECT_SOURCE_DIR}/data"
			"${RELEASE_DIR}/data/obs-plugins/input-overlay")

		COMMAND if $<CONFIG:Release>==1 ("${CMAKE_COMMAND}" -E copy
			"$<TARGET_FILE:input-overlay>"
			"${RELEASE_DIR}/obs-plugins/${ARCH_NAME}")

		# In Release mode, copy Qt image format plugins
		COMMAND if $<CONFIG:Release>==1 (
			"${CMAKE_COMMAND}" -E copy
			"${QTDIR}/plugins/imageformats/qjpeg.dll"
			"${RELEASE_DIR}/bin/${ARCH_NAME}/imageformats/qjpeg.dll")
		COMMAND if $<CONFIG:RelWithDebInfo>==1 (
			"${CMAKE_COMMAND}" -E copy
			"${QTDIR}/plugins/imageformats/qjpeg.dll"
			"${RELEASE_DIR}/bin/${ARCH_NAME}/imageformats/qjpeg.dll")

		# If config is RelWithDebInfo, package release files
		COMMAND if $<CONFIG:RelWithDebInfo>==1 (
			"${CMAKE_COMMAND}" -E make_directory
			"${RELEASE_DIR}/data/obs-plugins/input-overlay"
			"${RELEASE_DIR}/obs-plugins/${ARCH_NAME}")

		COMMAND if $<CONFIG:RelWithDebInfo>==1 ("${CMAKE_COMMAND}" -E copy_directory
			"${PROJECT_SOURCE_DIR}/data"
			"${RELEASE_DIR}/data/obs-plugins/input-overlay")

		COMMAND if $<CONFIG:RelWithDebInfo>==1 ("${CMAKE_COMMAND}" -E copy
			"$<TARGET_FILE:input-overlay>"
			"${RELEASE_DIR}/obs-plugins/${ARCH_NAME}")

		COMMAND if $<CONFIG:RelWithDebInfo>==1 ("${CMAKE_COMMAND}" -E copy
			"$<TARGET_PDB_FILE:input-overlay>"
			"${RELEASE_DIR}/obs-plugins/${ARCH_NAME}")

		# Copy to obs-studio dev environment for immediate testing
		COMMAND if $<CONFIG:Debug>==1 (
			"${CMAKE_COMMAND}" -E copy
				"$<TARGET_FILE:input-overlay>"
				"${LIBOBS_INCLUDE_DIR}/../${OBS_BUILDDIR_ARCH}/rundir/$<CONFIG>/obs-plugins/${ARCH_NAME}")

		COMMAND if $<CONFIG:Debug>==1 (
			"${CMAKE_COMMAND}" -E copy
				"$<TARGET_PDB_FILE:input-overlay>"
				"${LIBOBS_INCLUDE_DIR}/../${OBS_BUILDDIR_ARCH}/rundir/$<CONFIG>/obs-plugins/${ARCH_NAME}")

		COMMAND if $<CONFIG:Debug>==1 (
			"${CMAKE_COMMAND}" -E make_directory
				"${LIBOBS_INCLUDE_DIR}/../${OBS_BUILDDIR_ARCH}/rundir/$<CONFIG>/data/obs-plugins/input-overlay")

		COMMAND if $<CONFIG:Debug>==1 (
			"${CMAKE_COMMAND}" -E copy_directory
				"${PROJECT_SOURCE_DIR}/data"
				"${LIBOBS_INCLUDE_DIR}/../${OBS_BUILDDIR_ARCH}/rundir/$<CONFIG>/data/obs-plugins/input-overlay")
	)
	# --- End of sub-section ---

endif()
# --- End of section ---

# --- Linux-specific build settings and tasks ---
if(UNIX AND NOT APPLE)
	include(GNUInstallDirs)

	set_target_properties(input-overlay PROPERTIES PREFIX "")

	file(GLOB locale_files data/locale/*.ini)

        set(CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS
		OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

	install(TARGETS input-overlay LIBRARY
		DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/obs-plugins"
		PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

	install(FILES ${locale_files}
		DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/obs/obs-plugins/input-overlay/locale")
endif()
# --- End of section ---
