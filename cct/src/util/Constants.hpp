/**
 * Created by universal on 21.07.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */
#pragma once

#define MSG_SAVING_ERROR				"CCL encountered fatal errors while saving!"
#define MSG_LOADING_ERROR				"CCL encountered fatal errors while loading!"
#define MSG_SAVE_SUCESS(amount, time)	("Successfully wrote " << amount << " Element(s) in " << time\
		<< "ms")
#define MSG_LOAD_SUCCESS(amount,time)   ("Successfully loaded " << amount << " Element(s) in " << time\
		<< "ms")
#define MSG_CONFIG_EMPTY				"Target config file is empty"
#define MSG_CONFIG_CORRUPT				"Config file is missing first id entry. Possible corrupt file?"
#define MSG_INVALID_TYPE(id,type)		(id << " has invalid type " << type)
#define MSG_NOTHING_TO_SAVE				"Nothing to save"

#define DIALOG_NEW_ELEMENT				"New Element"
#define DIAOG_SETUP						""

#define ERROR_ID_NOT_UNIQUE				"Element ID must be unique"
#define ERROR_KEY_CODE_INVALID			"0x0 is not a valid keycode"
#define ERROR_SELECTION_EMPTY			"Selection was empty"

#define LABEL_TEXTURE_PATH				"Enter the path to the texture file:"
#define LABEL_DEFAULT_WIDTH				
