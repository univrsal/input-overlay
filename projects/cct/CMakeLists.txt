project(io-cct)

if (DEBUG)
    add_compile_definitions(_DEBUG)
endif ()

macro(use_cxx14)
    if (CMAKE_VERSION VERSION_LESS "3.1")
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")
        endif ()
    else ()
        set(CMAKE_CXX_STANDARD 14)
    endif ()
endmacro(use_cxx14)

string(TIMESTAMP TODAY "%Y.%m.%d %H:%M")
add_definitions(-DTIMESTAMP="${TODAY}")

use_cxx14()
add_compile_definitions(LGP_ENABLE_JSON)

if (MSVC)
    find_path(SDL2_INCLUDE_DIRS SDL.h)
    find_library(SDL2_LIBRARIES SDL2)

    find_path(SDL2TTF_INCLUDE_DIRS SDL_ttf.h)
    find_library(SDL2TTF_LIBRARIES SDL2_ttf)

    find_path(SDL2IMAGE_INCLUDE_DIRS SDL_image.h)
    find_library(SDL2IMAGE_LIBRARIES SDL2_image)
else ()
    set(CMAKE_C_FLAGS_DEBUG "-D_DEBUG")
    SET(GCC_FLAGS "-fpermissive")
    INCLUDE(FindPkgConfig)

    PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
    PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
    PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf)
endif ()

set(APP_ICON_RC "${CMAKE_CURRENT_SOURCE_DIR}/res/icon.ico")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_FLAGS}")

set(io-cct_SOURCES
        main.cpp
        src/tool.cpp
        src/tool.hpp
        src/config.hpp
        src/config.cpp
        src/util/constants.hpp
        src/util/font_helper.cpp
        src/util/font_helper.hpp
        src/util/palette.cpp
        src/util/palette.hpp
        src/util/sdl_helper.cpp
        src/util/sdl_helper.hpp
        src/util/texture.cpp
        src/util/texture.hpp
        src/util/util.cpp
        src/util/util.hpp
        src/util/coordinate_system.cpp
        src/util/coordinate_system.hpp
        src/util/notifier.cpp
        src/util/notifier.hpp
        src/util/localization.cpp
        src/util/localization.hpp
        src/dialog/dialog.hpp
        src/dialog/dialog.cpp
        src/dialog/dialog_setup.cpp
        src/dialog/dialog_setup.hpp
        src/dialog/dialog_help.cpp
        src/dialog/dialog_help.hpp
        src/dialog/dialog_new_element.cpp
        src/dialog/dialog_new_element.hpp
        src/dialog/dialog_element_type.cpp
        src/dialog/dialog_element_type.hpp
        src/dialog/dialog_element_settings.cpp
        src/dialog/dialog_element_settings.hpp
        src/dialog/elements/gui_element.cpp
        src/dialog/elements/gui_element.hpp
        src/dialog/elements/label.cpp
        src/dialog/elements/label.hpp
        src/dialog/elements/atlas_selector.cpp
        src/dialog/elements/atlas_selector.hpp
        src/dialog/elements/textbox.cpp
        src/dialog/elements/textbox.hpp
        src/dialog/elements/button.cpp
        src/dialog/elements/button.hpp
        src/dialog/elements/checkbox.cpp
        src/dialog/elements/checkbox.hpp
        src/dialog/elements/combobox.cpp
        src/dialog/elements/combobox.hpp
        src/element/element.cpp
        src/element/element.hpp
        src/element/element_texture.cpp
        src/element/element_texture.hpp
        src/element/element_button.cpp
        src/element/element_button.hpp
        src/element/element_analog_stick.cpp
        src/element/element_analog_stick.hpp
        src/element/element_scroll_wheel.cpp
        src/element/element_scroll_wheel.hpp
        src/element/element_mouse_movement.cpp
        src/element/element_mouse_movement.hpp
        src/element/element_trigger.cpp
        src/element/element_trigger.hpp
        src/element/element_gamepad_id.cpp
        src/element/element_gamepad_id.hpp
        src/element/element_dpad.cpp
        src/element/element_dpad.hpp
        ${JSON_11_SOURCE}
        )

add_executable(io-cct ${io-cct_SOURCES})

include_directories(${SDL2_INCLUDE_DIRS}
    ${SDL2IMAGE_INCLUDE_DIRS}
    ${SDL2TTF_INCLUDE_DIRS}
    ${COMMON_HEADERS}
    ${JSON_11_HEADER}
    )

message(STATUS "SKFSD:F:SJDFD:FJ ${JSON_11_HEADER}")

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES})

if (MSVC)
    get_filename_component(SDL2_BINARYDIR ${SDL2_LIBRARIES} DIRECTORY INTERNAL)
    get_filename_component(SDL2_IMAGE_BINARYDIR ${SDL2IMAGE_LIBRARIES} DIRECTORY INTERNAL)
    get_filename_component(SDL2_TTF_BINARYDIR ${SDL2TTF_LIBRARIES} DIRECTORY INTERNAL)

    # Copy dependencies
    add_custom_command(TARGET io-cct POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_BINARYDIR}/SDL2.dll"
        "${SDL2_IMAGE_BINARYDIR}/zlib1.dll"
        "${SDL2_IMAGE_BINARYDIR}/SDL2_image.dll"
        "${SDL2_IMAGE_BINARYDIR}/libwebp-7.dll"
        "${SDL2_IMAGE_BINARYDIR}/libtiff-5.dll"
        "${SDL2_IMAGE_BINARYDIR}/libpng16-16.dll"
        "${SDL2_IMAGE_BINARYDIR}/libjpeg-9.dll"
        "${SDL2_TTF_BINARYDIR}/SDL2_ttf.dll"
        "${SDL2_TTF_BINARYDIR}/libfreetype-6.dll"
        $<TARGET_FILE_DIR:io-cct>)

# Copy assets
add_custom_command(TARGET io-cct POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_CURRENT_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/../res)
else()
# Copy assets
add_custom_command(TARGET io-cct POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_CURRENT_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)
endif()
