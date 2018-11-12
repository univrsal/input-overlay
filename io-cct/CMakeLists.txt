cmake_minimum_required(VERSION 2.8)
project(io-cct)

file (STRINGS "build.txt" BUILD_NUMBER_)
add_definitions(-DBUILD_NUMBER=${BUILD_NUMBER_})

macro(use_cxx14)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")
    endif ()
  else ()
    set (CMAKE_CXX_STANDARD 14)
  endif ()
endmacro(use_cxx14)

use_cxx14()

if(MSVC)
	find_path(SDL2_INCLUDE_DIRS SDL.h)
	find_library(SDL2_LIBRARIES SDL2)

	find_path(SDLTTF2_INCLUDE_DIRS SDL_ttf.h)
	find_library(SDLTTF2_LIBRARIES SDL2_ttf)

	find_path(SDL2IMAGE_INCLUDE_DIRS SDL_image.h)
	find_library(SDL2IMAGE_LIBRARIES SDL2_image)
else()
	set(CMAKE_C_FLAGS_DEBUG "-D_DEBUG")
	SET(GCC_FLAGS "-fpermissive")
	INCLUDE(FindPkgConfig)

	PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
	PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
	PKG_SEARCH_MODULE(SDLTTF2 REQUIRED SDL2_ttf)
endif()

set(APP_ICON_RC "${CMAKE_CURRENT_SOURCE_DIR}/res/icon.ico")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_FLAGS}")

set(io-cct_SOURCES
    ../ccl/ccl.cpp
    ../ccl/ccl.hpp
    ../libuiohook/include/uiohook.h
    ../io-obs/util/layout_constants.hpp
    main.cpp
    src/Tool.cpp
    src/Tool.hpp
    src/Config.cpp
    src/Config.hpp
    src/util/Constants.hpp
    src/util/FontHelper.cpp
    src/util/FontHelper.hpp
    src/util/Palette.cpp
    src/util/Palette.hpp
    src/util/SDL_Helper.cpp
    src/util/SDL_Helper.hpp
    src/util/Texture.cpp
    src/util/Texture.hpp
    src/util/Util.cpp
    src/util/Util.hpp
    src/util/CoordinateSystem.cpp
    src/util/CoordinateSystem.hpp
    src/util/Notifier.cpp
    src/util/Notifier.hpp
    src/util/Localization.cpp
    src/util/Localization.hpp
    src/dialog/Dialog.hpp
    src/dialog/Dialog.cpp
    src/dialog/DialogSetup.cpp
    src/dialog/DialogSetup.hpp
    src/dialog/DialogHelp.cpp
    src/dialog/DialogHelp.hpp
    src/dialog/DialogNewElement.cpp
    src/dialog/DialogNewElement.hpp
    src/dialog/DialogElementType.cpp
    src/dialog/DialogElementType.hpp
    src/dialog/DialogElementSettings.cpp
    src/dialog/DialogElementSettings.hpp
    src/dialog/elements/GuiElement.cpp
    src/dialog/elements/GuiElement.hpp
    src/dialog/elements/Label.cpp
    src/dialog/elements/Label.hpp
    src/dialog/elements/AtlasSelector.cpp
    src/dialog/elements/AtlasSelector.hpp
    src/dialog/elements/Textbox.cpp
    src/dialog/elements/Textbox.hpp
    src/dialog/elements/Button.cpp
    src/dialog/elements/Button.hpp
    src/dialog/elements/Checkbox.cpp
    src/dialog/elements/Checkbox.hpp
    src/dialog/elements/Combobox.cpp
    src/dialog/elements/Combobox.hpp
    src/element/Element.cpp
    src/element/Element.hpp
    src/element/ElementTexture.cpp
    src/element/ElementTexture.hpp
    src/element/ElementText.cpp
    src/element/ElementText.hpp
    src/element/ElementButton.cpp
    src/element/ElementButton.hpp
    src/element/ElementAnalogStick.cpp
    src/element/ElementAnalogStick.hpp
    src/element/ElementScrollWheel.cpp
    src/element/ElementScrollWheel.hpp
    src/element/ElementMouseMovement.cpp
    src/element/ElementMouseMovement.hpp
    src/element/ElementTrigger.cpp
    src/element/ElementTrigger.hpp
    src/element/ElementGamepadID.cpp
    src/element/ElementGamepadID.hpp
    src/element/ElementDPad.cpp
    src/element/ElementDPad.hpp
    )

add_executable(io-cct ${io-cct_SOURCES})

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDLTTF2_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDLTTF2_LIBRARIES})
