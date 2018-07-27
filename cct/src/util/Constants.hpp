/**
 * Created by universal on 21.07.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */
#pragma once

 /* SDL Error messages*/
#define SDL_INIT_FAILED					"Initialization of SDL failed! Error: %s\n"
#define SDL_CREATE_WINDOW_FAILED		"Creating SDL Window failed! Error: %s\n"
#define SDL_LOAD_WINDOW_ICON_FAILED		"Loading window icon failed! Error: %s\n"
#define SDL_CREATE_RENDERER_FAILED		"Creating SDL Renderer failed! Error: %s\n"
#define SDL_TTF_INIT_FAILED				"Initializing SDL_ttf failed! Error: %s\n"
#define SDL_FONT_LOADING_FAILED			"Couldn't load fonts!\n"
#define SDL_IMAGE_TO_SURFACE			"ERROR: Couldn't create surface %s! SDL_Error: %s\n"
#define SDL_SURFACE_TO_TEXTURE_FAILED	"ERROR: Couldn't load image %s! SDL_Error: %s\n"
#define SDL_TEXT_TO_TEXTURE				"Error while creating texture from surface in text rendering! ERROR: %s\n"
#define SDL_TEXT_TO_SURFACE				"Error while creating surface from font in text rendering! ERROR: %s\n"

/* Resources */
#define PATH_ROBOTO_FONT				"./roboto-regular.ttf"
#define PATH_UNICODE_FONT				"./antique-maru.ttf"

/* Notifier messages*/
#define MSG_SAVING_ERROR				"CCL encountered fatal errors while saving!"
#define MSG_LOADING_ERROR				"CCL encountered fatal errors while loading!"
#define MSG_SAVE_SUCESS(amount, time)	"Successfully wrote " << amount << " Element(s) in " << time\
		<< "ms"
#define MSG_LOAD_SUCCESS(amount,time)   "Successfully loaded " << amount << " Element(s) in " << time\
		<< "ms"
#define MSG_CONFIG_EMPTY				"Target config file is empty"
#define MSG_CONFIG_CORRUPT				"Config file is missing first id entry. Possible corrupt file?"
#define MSG_INVALID_TYPE(id,type)	    id << " has invalid type " << type
#define MSG_NOTHING_TO_SAVE				"Nothing to save"

/* Dialog titles*/
#define DIALOG_NEW_ELEMENT				"New Element"
#define DIALOG_SETUP					"Overlay setup"
#define DIALOG_HELP						"Help and about"
#define DIALOG_ELEMENT_SETTINGS			"Selected element settings"

/* Errors */
#define ERROR_ID_NOT_UNIQUE				"Element ID must be unique"
#define ERROR_KEY_CODE_INVALID			"0x0 is not a valid keycode"
#define ERROR_SELECTION_EMPTY			"Selection was empty"

/* Setup dialog */
#define LABEL_TEXTURE_PATH				"Enter the path to the texture file:"
#define LABEL_DEFAULT_WIDTH				"Default element width:"
#define LABEL_DEFAULT_HEIGHT			"Default element height:"
#define LABEL_CONFIG_PATH				"Enter config path for saving or loading:"
#define ERROR_INVALID_TEXTURE_PATH		"Invalid texture path!"
#define ERROR_INVALID_CONFIG_PATH		"Invalid config path!"

/* Buttons */
#define BUTTON_OK						"OK"
#define BUTTON_EXIT						"Exit"
#define BUTTON_CANCEL					"Cancel"

/* Misc */
#define ELEMENT_UNNAMED					"unnamed"
#define LABEL_X_AXIS					"X in pixels"
#define LABEL_Y_AXIS					"Y in pixels"
#define LABEL_SCALE(scale)				"Scale: " + scale + "x"

/* New Element Dialog */
#define LABEL_TEXTURE_SELECTION			"Texture selection"
#define LABEL_WIDTH 					"Width:"
#define LABEL_HEIGHT 					"Height:"
#define LABEL_U 						"U:"
#define LABEL_V 						"V:"

#define LABEL_KEY_CODE					"Key code:"
#define CHECKBOX_RECORD_KEYBIND			"Record keybind"
#define LABEL_ELEMENT_ID				"Element id:"

/* Selected element settings */
#define LABEL_X 					"X:"
#define LABEL_Y 						"Y:"
#define BUTTON_ADD_ELEMENT				"Add new element"
#define BUTTON_DELETE_ELEMENT			"Delete selected"
#define BUTTON_MODIFY_ELEMENT			"Modify selected"
#define BUTTON_SAVE_CONFIG				"Save config"
#define BUTTON_HELP						"Help"

/* Help and about dialog*/
#define LABEL_HELP_AND_ABOUT			"input-overlay config creation tool\
									   \nlicensed under the Mozilla Public Licence 2.0\
									   \ngithub.com/univrsal/input-overlay\
									   \n\n\
									     Thanks to:\
									   \nThe OBS Studio team and contributers\
									   \nobsproject.com\
									   \nThe SDL developers (also SDL_image and SDL_ttf)\
									   \nlibsdl.org\
									   \n\n\
									     Usage:\
									   \nScroll wheel: Zoom in/out\
									   \nLeft mouse: move elements\
									   \nRight mouse: move preview\
									   \n\n\
									     Some keys will work within the preview\
									   \nand light up when pressed"

/**
 * Created by universal on 21.07.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */
#pragma once

 /* SDL Error messages*/
#define SDL_INIT_FAILED					"Initialization of SDL failed! Error: %s\n"
#define SDL_CREATE_WINDOW_FAILED		"Creating SDL Window failed! Error: %s\n"
#define SDL_LOAD_WINDOW_ICON_FAILED		"Loading window icon failed! Error: %s\n"
#define SDL_CREATE_RENDERER_FAILED		"Creating SDL Renderer failed! Error: %s\n"
#define SDL_TTF_INIT_FAILED				"Initializing SDL_ttf failed! Error: %s\n"
#define SDL_FONT_LOADING_FAILED			"Couldn't load fonts!\n"
#define SDL_IMAGE_TO_SURFACE			"ERROR: Couldn't create surface %s! SDL_Error: %s\n"
#define SDL_SURFACE_TO_TEXTURE_FAILED	"ERROR: Couldn't load image %s! SDL_Error: %s\n"
#define SDL_TEXT_TO_TEXTURE				"Error while creating texture from surface in text rendering! ERROR: %s\n"
#define SDL_TEXT_TO_SURFACE				"Error while creating surface from font in text rendering! ERROR: %s\n"

/* Resources */
#define PATH_ROBOTO_FONT				"./roboto-regular.ttf"
#define PATH_UNICODE_FONT				"./antique-maru.ttf"

/* Notifier messages*/
#define MSG_SAVING_ERROR				"CCL encountered fatal errors while saving!"
#define MSG_LOADING_ERROR				"CCL encountered fatal errors while loading!"
#define MSG_SAVE_SUCESS(amount, time)	"Successfully wrote " << amount << " Element(s) in " << time\
		<< "ms"
#define MSG_LOAD_SUCCESS(amount,time)   "Successfully loaded " << amount << " Element(s) in " << time\
		<< "ms"
#define MSG_CONFIG_EMPTY				"Target config file is empty"
#define MSG_CONFIG_CORRUPT				"Config file is missing first id entry. Possible corrupt file?"
#define MSG_INVALID_TYPE(id,type)	    id << " has invalid type " << type
#define MSG_NOTHING_TO_SAVE				"Nothing to save"

/* Dialog titles*/
#define DIALOG_NEW_ELEMENT				"New Element"
#define DIALOG_SETUP					"Overlay setup"
#define DIALOG_HELP						"Help and about"
#define DIALOG_ELEMENT_SETTINGS			"Selected element settings"

/* Errors */
#define ERROR_ID_NOT_UNIQUE				"Element ID must be unique"
#define ERROR_KEY_CODE_INVALID			"0x0 is not a valid keycode"
#define ERROR_SELECTION_EMPTY			"Selection was empty"

/* Setup dialog */
#define LABEL_TEXTURE_PATH				"Enter the path to the texture file:"
#define LABEL_DEFAULT_WIDTH				"Default element width:"
#define LABEL_DEFAULT_HEIGHT			"Default element height:"
#define LABEL_CONFIG_PATH				"Enter config path for saving or loading:"
#define ERROR_INVALID_TEXTURE_PATH		"Invalid texture path!"
#define ERROR_INVALID_CONFIG_PATH		"Invalid config path!"

/* Buttons */
#define BUTTON_OK						"OK"
#define BUTTON_EXIT						"Exit"
#define BUTTON_CANCEL					"Cancel"

/* Misc */
#define ELEMENT_UNNAMED					"unnamed"
#define LABEL_X_AXIS					"X in pixels"
#define LABEL_Y_AXIS					"Y in pixels"
#define LABEL_SCALE(scale)				"Scale: " + scale + "x"

/* New Element Dialog */
#define LABEL_TEXTURE_SELECTION			"Texture selection"
#define LABEL_WIDTH 					"Width:"
#define LABEL_HEIGHT 					"Height:"
#define LABEL_U 						"U:"
#define LABEL_V 						"V:"

#define LABEL_KEY_CODE					"Key code:"
#define CHECKBOX_RECORD_KEYBIND			"Record keybind"
#define LABEL_ELEMENT_ID				"Element id:"

/* Selected element settings */
#define LABEL_X 					"X:"
#define LABEL_Y 						"Y:"
#define BUTTON_ADD_ELEMENT				"Add new element"
#define BUTTON_DELETE_ELEMENT			"Delete selected"
#define BUTTON_MODIFY_ELEMENT			"Modify selected"
#define BUTTON_SAVE_CONFIG				"Save config"
#define BUTTON_HELP						"Help"

/* Help and about dialog*/
#define LABEL_HELP_AND_ABOUT			"input-overlay config creation tool\
									   \nlicensed under the Mozilla Public Licence 2.0\
									   \ngithub.com/univrsal/input-overlay\
									   \n\n\
									     Thanks to:\
									   \nThe OBS Studio team and contributers\
									   \nobsproject.com\
									   \nThe SDL developers (also SDL_image and SDL_ttf)\
									   \nlibsdl.org\
									   \n\n\
									     Usage:\
									   \nScroll wheel: Zoom in/out\
									   \nLeft mouse: move elements\
									   \nRight mouse: move preview\
									   \n\n\
									     Some keys will work within the preview\
									   \nand light up when pressed"

/**
 * Created by universal on 21.07.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */
#pragma once

 /* SDL Error messages*/
#define SDL_INIT_FAILED					"Initialization of SDL failed! Error: %s\n"
#define SDL_CREATE_WINDOW_FAILED		"Creating SDL Window failed! Error: %s\n"
#define SDL_LOAD_WINDOW_ICON_FAILED		"Loading window icon failed! Error: %s\n"
#define SDL_CREATE_RENDERER_FAILED		"Creating SDL Renderer failed! Error: %s\n"
#define SDL_TTF_INIT_FAILED				"Initializing SDL_ttf failed! Error: %s\n"
#define SDL_FONT_LOADING_FAILED			"Couldn't load fonts!\n"
#define SDL_IMAGE_TO_SURFACE			"ERROR: Couldn't create surface %s! SDL_Error: %s\n"
#define SDL_SURFACE_TO_TEXTURE_FAILED	"ERROR: Couldn't load image %s! SDL_Error: %s\n"
#define SDL_TEXT_TO_TEXTURE				"Error while creating texture from surface in text rendering! ERROR: %s\n"
#define SDL_TEXT_TO_SURFACE				"Error while creating surface from font in text rendering! ERROR: %s\n"

/* Resources */
#define PATH_ROBOTO_FONT				"./roboto-regular.ttf"
#define PATH_UNICODE_FONT				"./antique-maru.ttf"

/* Notifier messages*/
#define MSG_SAVING_ERROR				"CCL encountered fatal errors while saving!"
#define MSG_LOADING_ERROR				"CCL encountered fatal errors while loading!"
#define MSG_SAVE_SUCESS(amount, time)	"Successfully wrote " << amount << " Element(s) in " << time\
		<< "ms"
#define MSG_LOAD_SUCCESS(amount,time)   "Successfully loaded " << amount << " Element(s) in " << time\
		<< "ms"
#define MSG_CONFIG_EMPTY				"Target config file is empty"
#define MSG_CONFIG_CORRUPT				"Config file is missing first id entry. Possible corrupt file?"
#define MSG_INVALID_TYPE(id,type)	    id << " has invalid type " << type
#define MSG_NOTHING_TO_SAVE				"Nothing to save"

/* Dialog titles*/
#define DIALOG_NEW_ELEMENT				"New Element"
#define DIALOG_SETUP					"Overlay setup"
#define DIALOG_HELP						"Help and about"
#define DIALOG_ELEMENT_SETTINGS			"Selected element settings"

/* Errors */
#define ERROR_ID_NOT_UNIQUE				"Element ID must be unique"
#define ERROR_KEY_CODE_INVALID			"0x0 is not a valid keycode"
#define ERROR_SELECTION_EMPTY			"Selection was empty"

/* Setup dialog */
#define LABEL_TEXTURE_PATH				"Enter the path to the texture file:"
#define LABEL_DEFAULT_WIDTH				"Default element width:"
#define LABEL_DEFAULT_HEIGHT			"Default element height:"
#define LABEL_CONFIG_PATH				"Enter config path for saving or loading:"
#define ERROR_INVALID_TEXTURE_PATH		"Invalid texture path!"
#define ERROR_INVALID_CONFIG_PATH		"Invalid config path!"

/* Buttons */
#define BUTTON_OK						"OK"
#define BUTTON_EXIT						"Exit"
#define BUTTON_CANCEL					"Cancel"

/* Misc */
#define ELEMENT_UNNAMED					"unnamed"
#define LABEL_X_AXIS					"X in pixels"
#define LABEL_Y_AXIS					"Y in pixels"
#define LABEL_SCALE(scale)				"Scale: " + scale + "x"

/* New Element Dialog */
#define LABEL_TEXTURE_SELECTION			"Texture selection"
#define LABEL_WIDTH 					"Width:"
#define LABEL_HEIGHT 					"Height:"
#define LABEL_U 						"U:"
#define LABEL_V 						"V:"

#define LABEL_KEY_CODE					"Key code:"
#define CHECKBOX_RECORD_KEYBIND			"Record keybind"
#define LABEL_ELEMENT_ID				"Element id:"

/* Selected element settings */
#define LABEL_X 					"X:"
#define LABEL_Y 						"Y:"
#define BUTTON_ADD_ELEMENT				"Add new element"
#define BUTTON_DELETE_ELEMENT			"Delete selected"
#define BUTTON_MODIFY_ELEMENT			"Modify selected"
#define BUTTON_SAVE_CONFIG				"Save config"
#define BUTTON_HELP						"Help"

/* Help and about dialog*/
#define LABEL_HELP_AND_ABOUT			"input-overlay config creation tool\
									   \nlicensed under the Mozilla Public Licence 2.0\
									   \ngithub.com/univrsal/input-overlay\
									   \n\n\
									     Thanks to:\
									   \nThe OBS Studio team and contributers\
									   \nobsproject.com\
									   \nThe SDL developers (also SDL_image and SDL_ttf)\
									   \nlibsdl.org\
									   \n\n\
									     Usage:\
									   \nScroll wheel: Zoom in/out\
									   \nLeft mouse: move elements\
									   \nRight mouse: move preview\
									   \n\n\
									     Some keys will work within the preview\
									   \nand light up when pressed"

