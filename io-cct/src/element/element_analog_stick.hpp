/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#define STICK_RESET 500

#include "element_texture.hpp"
#include "../util/util.hpp"

class ElementAnalogStick : public element_texture {
public:
	ElementAnalogStick()
		: element_texture(),
		  m_static_scaled(),
		  m_stick()
	{
		/* NO-OP */
	}

	ElementAnalogStick(const std::string &id, SDL_Point pos, SDL_Rect mapping, element_side side, uint8_t radius,
	                   uint8_t z);

	SDL_Rect *get_abs_dim(coordinate_system *cs) override;

	element_error is_valid(notifier *n, sdl_helper *h) override;

	void draw(texture *atlas, coordinate_system *cs, bool selected, bool alpha) override;

	void write_to_file(ccl_config *cfg, SDL_Point *default_dim, uint8_t &flags) override;

	void update_settings(dialog_new_element *dialog) override;

	void update_settings(dialog_element_settings *dialog) override;

	void handle_event(SDL_Event *event, sdl_helper *helper) override;

	element_side get_stick() const
	{
		return m_stick;
	}

	uint16_t get_radius() const
	{
		return m_radius;
	}

	static ElementAnalogStick *read_from_file(ccl_config *file, const std::string &id, SDL_Point *default_dim);

private:
	SDL_Rect m_static_scaled; /* Position without input movement for display */
	element_side m_stick;
	bool m_pressed = false;
	float m_x_axis = 0;
	float m_y_axis = 0;
	uint16_t m_radius = 0;

	Timer m_movement_reset;
};
