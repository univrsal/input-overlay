/**
 * Created by universal on 27.05.2018.
 * This file is part of reloded which is licenced
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "util/SDL_helper.hpp"
#include <string>
#include <SDL.h>
#include <memory>
#include <vector>

class SDL_helper;

namespace Overlay
{

	enum ElementType
	{
		TEXTURE,
		BUTTON_KEYBOARD,
		BUTTON_GAMEPAD,
		MOUSE_MOVEMENT,
		ANALOG_STICK,
		TEXT,
		DPAD,
		DPAD_STICK
	};

	class Element
	{
	public:
		Element(ElementType t, SDL_Rect dim, SDL_Rect map, uint16_t keycode)
		{
			m_type = t;
			m_dimensions = dim;
			m_texture_mapping = map;
			m_keycode = keycode;
		}
	private:
		ElementType m_type;
		SDL_Rect m_dimensions;
		SDL_Rect m_texture_mapping;
		uint16_t m_keycode;
	};

	class Config
	{
	public:
		Config(const std::string * texture, const std::string * config, SDL_helper * h);

		void draw_elements(void);

		void handle_events(SDL_Event * e);
		Element * get_selected(void);

		Element * m_selected = nullptr;
		const std::string * m_texture_path;
		const std::string * m_config_path;
		std::vector<std::unique_ptr<Element>> m_elements;
	private:
		SDL_helper * m_helper;
		SDL_Point m_origin;

		int m_scale_f = 1;

		bool m_dragging = false;
		SDL_Point m_drag_offset;
	};

};
