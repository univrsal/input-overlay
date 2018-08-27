/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include "../../../io-obs/util/layout_constants.hpp"
#include <string>
#include <SDL.h>

class CoordinateSystem;

class ccl_config;

class Texture;

/* Base class for display elements
 */
class Element
{
public:
	virtual void draw(Texture * atlas, CoordinateSystem * cs, bool selected, bool alpha) = 0;

	virtual void write_to_file(ccl_config * cfg, SDL_Point * default_dim);

	virtual SDL_Rect * get_abs_dim(CoordinateSystem * cs) = 0;

	SDL_Rect * get_dim(void);

	void set_pos(int x, int y);

	void set_id(std::string id);

	void set_z_level(uint8_t z);

	uint8_t get_z_level(void);

	std::string * get_id(void);

	int get_x();
	int get_y();
	int get_w();
	int get_h();

	ElementType get_type(void);

	virtual void handle_event(SDL_Event * event) = 0;

	static Element * read_from_file(ccl_config * file, std::string id, ElementType t, SDL_Point * default_dim);
	static bool valid_type(int t);
protected:
	Element(ElementType t, std::string id, SDL_Point pos, uint8_t z);

	static inline SDL_Rect read_mapping(ccl_config * file, std::string id, SDL_Point * default_dim);
	static inline SDL_Point read_position(ccl_config * file, std::string id);
	static inline uint8_t read_layer(ccl_config * file, std::string id);

	ElementType m_type;
	SDL_Point m_position; /* Final position in overlay */
	SDL_Rect m_dimensions;
	SDL_Rect m_dimensions_scaled;

	uint8_t m_scale = 0; /* Currently used scale factor */

	uint8_t m_z_level = 0; /* Determines draw and selection order */
	std::string m_id;
};
