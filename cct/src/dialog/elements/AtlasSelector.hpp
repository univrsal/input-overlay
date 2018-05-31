/**
 * Created by universal on 30.05.2018.
 * This file is part of reloded which is licenced
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "GuiElement.hpp"
#include "../Dialog.hpp"
#include "../../util/Texture.hpp"
#include <string>

class Dialog;

class Texture;

class GuiElement;

class AtlasSelector : public GuiElement
{
public:
	AtlasSelector(int8_t id, int x, int y, int w, int h, Texture * t, Dialog * parent);

	~AtlasSelector();

	void close(void);

	void draw_foreground(void);

	void draw_background(void);

	bool handle_events(SDL_Event * event);

	SDL_Rect * get_selection();
private:
	SDL_Rect m_selection;
};
