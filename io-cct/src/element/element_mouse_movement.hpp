/**
 * Created by universal on 05.09.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "element_texture.hpp"

class ElementMouseMovement : public element_texture {
public:
	ElementMouseMovement()
		: element_texture(),
		  m_type()
	{
		/* NO-OP */
	};

	ElementMouseMovement(const std::string &id, SDL_Point pos, SDL_Rect mapping, mouse_movement type,
	                     uint16_t radius, uint8_t z);

	element_error is_valid(notifier *n, sdl_helper *h) override;

	void write_to_file(ccl_config *cfg, SDL_Point *default_dim, uint8_t &layout_flags) override;

	void update_settings(dialog_new_element *dialog) override;

	mouse_movement get_mouse_type() const;

	static ElementMouseMovement *read_from_file(ccl_config *file, const std::string &id, SDL_Point *default_dim);

	uint16_t get_radius() const;

private:
	mouse_movement m_type;
	uint16_t m_radius = 0;
};
