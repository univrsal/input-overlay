/**
 * Created by universal on 02.05.2018.
 * This file is part of reloded which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "../../util/CoordinateSystem.hpp"
#include "../../util/SDL_helper.hpp"
#include "../../util/Texture.hpp"
#include <SDL.h>
#include "GuiElement.hpp"
#include "../Dialog.hpp"

class SDL_helper;

class CoordinateSystem;

class Texture;

class AtlasSelector : public GuiElement
{
public:
	AtlasSelector(int8_t id, int x, int y, int w, int h, Texture * t, Dialog * parent);

	~AtlasSelector();

	void close(void);

	void init(Dialog *parent, SDL_Rect dim, int8_t id);

	void draw_foreground(void);

	void draw_background(void);

	bool handle_events(SDL_Event * event);

	uint8_t get_cursor(void);

	void set_selection(SDL_Rect * r) { m_cs->set_selection(r); }
private:
	void resize();
	
	Texture * m_atlas; /* Is loaded in Config, and will be deleted there */
	CoordinateSystem * m_cs;

	bool m_have_cursors = false;
};
