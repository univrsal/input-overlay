project(io-cct)

find_path(SDL_INCLUDE_DIR SDL.h)
find_library(SDL_LIB SDL2)

find_path(SDL_TTF_INCLUDE_DIR SDL_ttf.h)
find_library(SDL_TTF_LIB SDL2_ttf)

find_path(SDL_IMG_INCLUDE_DIR SDL_image.h)
find_library(SDL_IMG_LIB SDL2_image)

set(io-cct_SOURCES
	main.cpp
	src/util/FontHelper.cpp
	src/util/FontHelper.hpp
	src/util/Palette.cpp
	src/util/Palette.hpp
	src/util/SDL_helper.cpp
	src/util/SDL_helper.hpp
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
	src/util/CoordinateSystem.cpp
	src/util/CoordinateSystem.hpp
	src/util/Notifier.cpp
	src/util/Notifier.hpp)

include_directories(${SDL_INCLUDE_DIR}
	${SDL_TTF_INCLUDE_DIR}
	${SDL_IMG_INCLUDE_DIR})

add_executable(io-cct ${io-cct_SOURCES})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL_LIB} ${SDL_IMG_LIB} ${SDL_TTF_LIB})