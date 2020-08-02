# Functions for generating external plugins

set(EXTERNAL_PLUGIN_OUTPUT_DIR "${LibObs_DIR}/../rundir")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(OBS_ARCH_NAME "64bit")
    set(OBS_BUILDDIR_ARCH "build64")
else()
    set(OBS_ARCH_NAME "32bit")
    set(OBS_BUILDDIR_ARCH "build32")
endif()

# Fix XCode includes to ignore warnings on system includes
function(target_include_directories_system _target)
  if(XCODE)
    foreach(_arg ${ARGN})
      if("${_arg}" STREQUAL "PRIVATE" OR "${_arg}" STREQUAL "PUBLIC" OR "${_arg}" STREQUAL "INTERFACE")
        set(_scope ${_arg})
      else()
        target_compile_options(${_target} ${_scope} -isystem${_arg})
      endif()
    endforeach()
  else()
    target_include_directories(${_target} SYSTEM ${_scope} ${ARGN})
  endif()
endfunction()

function(install_external_plugin_data_internal target source_dir)
	install(DIRECTORY ${source_dir}/
		DESTINATION "data/obs-plugins/${target}"
		USE_SOURCE_PERMISSIONS)
	add_custom_command(TARGET ${target} POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy_directory
			"${CMAKE_CURRENT_SOURCE_DIR}/${source_dir}" "${EXTERNAL_PLUGIN_OUTPUT_DIR}/$<CONFIGURATION>/data/obs-plugins/${target}"
		VERBATIM)
endfunction()

# Installs data
# 'target' is the destination target project being installed to
# 'data_loc' specifies the directory of the data
function(install_external_plugin_data target data_loc)
	install_external_plugin_data_internal(${target} ${data_loc})
endfunction()

# Installs an additional binary to a target
# 'target' is the destination target project being installed to
# 'additional_target' specifies the additional binary
function(install_external_plugin_additional target additional_target)
	if(APPLE)
		set(_bit_suffix "")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(_bit_suffix "64bit/")
	else()
		set(_bit_suffix "32bit/")
	endif()

	set_target_properties(${additional_target} PROPERTIES
		PREFIX "")

	install(TARGETS ${additional_target}
		LIBRARY DESTINATION "bin"
		RUNTIME DESTINATION "bin")
	install(FILES $<TARGET_PDB_FILE:${additional_target}> DESTINATION bin OPTIONAL)

	add_custom_command(TARGET ${additional_target} POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy
			"$<TARGET_FILE:${additional_target}>"
			"${EXTERNAL_PLUGIN_OUTPUT_DIR}/$<CONFIGURATION>/${target}/${_bit_suffix}$<TARGET_FILE_NAME:${additional_target}>"
		VERBATIM)
    	add_custom_command(TARGET ${additional_target} POST_BUILD
    		COMMAND ${CMAKE_COMMAND} -E copy_if_different
        		$<TARGET_PDB_FILE:${additional_target}>
		        "${EXTERNAL_PLUGIN_OUTPUT_DIR}/$<CONFIGURATION>/${target}/${_bit_suffix}")
endfunction()

# Installs the binary of the target
# 'target' is the target project being installed
function(install_external_plugin target)
	install_external_plugin_additional(obs-plugins ${target})
endfunction()

# Installs the binary and data of the target
# 'target' is the destination target project being installed to
function(install_external_plugin_with_data target data_loc)
	install_external_plugin(${target})
	install_external_plugin_data(${target} ${data_loc})
endfunction()