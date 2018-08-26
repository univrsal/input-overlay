cmake_minimum_required(VERSION 2.8)
project(io-cct)

file (STRINGS "build.txt" BUILD_NUMBER_)
add_definitions(-DBUILD_NUMBER=${BUILD_NUMBER_})

macro(use_cxx11)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
    endif ()
  else ()
    set (CMAKE_CXX_STANDARD 11)
  endif ()
endmacro(use_cxx11)

use_cxx11()

if(MSVC)
	find_path(SDL2_INCLUDE_DIRS SDL.h)
	find_library(SDL2_LIBRARIES SDL2)

	find_path(SDLTTF2_INCLUDE_DIRS SDL_ttf.h)
	find_library(SDLTTF2_LIBRARIES SDL2_ttf)

	find_path(SDL2IMAGE_INCLUDE_DIRS SDL_image.h)
	find_library(SDL2IMAGE_LIBRARIES SDL2_image)
else()
	SET(GCC_FLAGS "-fpermissive")
	INCLUDE(FindPkgConfig)

	PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
	PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
	PKG_SEARCH_MODULE(SDLTTF2 REQUIRED SDL2_ttf)
endif()

set(APP_ICON_RC "${CMAKE_CURRENT_SOURCE_DIR}/res/icon.ico")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_FLAGS}")

set(io-cct_SOURCES
	main.cpp
	src/util/Constants.hpp
	src/util/FontHelper.cpp
	src/util/FontHelper.hpp
	src/util/Palette.cpp
	src/util/Palette.hpp
	src/util/SDL_Helper.cpp
	src/util/SDL_Helper.hpp
	src/util/Texture.cpp
	src/util/Texture.hpp
	src/dialog/Dialog.hpp
	src/dialog/Dialog.cpp
	src/dialog/elements/GuiElement.cpp
	src/dialog/elements/GuiElement.hpp
	src/dialog/elements/Label.cpp
	src/dialog/elements/Label.hpp
	src/dialog/elements/AtlasSelector.cpp
	src/dialog/elements/AtlasSelector.hpp
	src/Tool.cpp
	src/Tool.hpp
	src/dialog/DialogSetup.cpp
	src/dialog/DialogSetup.hpp
	src/dialog/elements/Textbox.cpp
	src/dialog/elements/Textbox.hpp
	src/dialog/elements/Button.cpp
	src/dialog/elements/Button.hpp
	src/dialog/elements/Checkbox.cpp
	src/dialog/elements/Checkbox.hpp
	src/dialog/elements/Combobox.cpp
	src/dialog/elements/Combobox.hpp
	src/Config.cpp
	src/Config.hpp
	src/dialog/DialogElementSettings.cpp
	src/dialog/DialogElementSettings.hpp
	../ccl/ccl.cpp
	../ccl/ccl.hpp
	../libuiohook/include/uiohook.h
	src/dialog/DialogHelp.cpp
	src/dialog/DialogHelp.hpp
	src/dialog/DialogNewElement.cpp
	src/dialog/DialogNewElement.hpp
	src/dialog/DialogElementType.cpp
	src/dialog/DialogElementType.hpp
	src/util/CoordinateSystem.cpp
	src/util/CoordinateSystem.hpp
	src/util/Notifier.cpp
	src/util/Notifier.hpp
	src/util/Localization.cpp
	src/util/Localization.hpp)

add_executable(io-cct ${io-cct_SOURCES})

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDLTTF2_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDLTTF2_LIBRARIES})
