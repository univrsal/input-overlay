/**
 * Created by universal on 02.05.2018.
 * This file is part of reloded which is licenced
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

#define SIZE_LEFT 0
#define SIZE_RIGHT 1
#define SIZE_TOP 2
#define SIZE_BOTTOM 3
#define SIZE_MOVE 4
#define SIZE_NONE 5

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

	SDL_Rect * get_selection();

	uint8_t get_cursor(void);
private:
	void resize();

	inline bool in_range(int a, int b, int range);
	void mouse_state(SDL_Event * event);
	uint8_t m_size_mode = SIZE_NONE;

	SDL_Rect m_selection = { 0, 0, 0, 0 };
	SDL_Point m_selection_a;

	bool m_selecting = false;
	bool m_sizing = false;
	Texture * m_atlas;
	CoordinateSystem * m_cs;

	bool m_have_cursors = false;
};
