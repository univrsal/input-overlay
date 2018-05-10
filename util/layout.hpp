#ifndef LAYOUT_HPP
#define LAYOUT_HPP

#ifdef LINUX
#include <stdint.h>
#endif
#include <vector>
#include <obs-module.h>
#include <string.h>

extern "C" {
#include <graphics/image-file.h>
}

#include "../ccl/ccl.hpp"

/* Texture constants */
#define INNER_BORDER 3
#define OUTTER_BORDER 1

/**
 * This file is part of input-overlay
 * which is licenced under the MIT licence.
 * See LICENCE or https://mit-license.org
 * github.com/univrsal/input-overlay
 */

namespace Layout {

enum ElementType
{
	ELEMENT_TEXTURE,
	ELEMENT_KEY,
	ELEMENT_MOUSE_MOVEMENT,
	ELEMENT_ANALOG_STICK,
	ELEMENT_TRIGGER,
	ELEMENT_TEXT
};

class Element
{
public:
	Element(ElementType type)
	{
		m_type = type;
	}

	virtual void load(ccl_config * cfg, std::string id) = 0;
	virtual void draw(gs_effect_t *effect, gs_image_file_t * m_image/* Keystate */) = 0;

protected:
	int m_xpos, m_ypos;
	int m_u, m_v;
	int m_width, m_height;
	ElementType m_type;
};

class ElementTexture : public Element
{
	void load(ccl_config * cfg, std::string id);
	void draw(gs_effect_t * effect, gs_image_file_t * image);
};

class ElementButton : public Element
{
	void load(ccl_config * cfg, std::string id);
	void draw(gs_effect_t * effect, gs_image_file_t * image);
private:
	int m_keycode;
};

class Overlay
{
public:
	Overlay(std::string ini, std::string texture);

	void load_cfg(std::string path);
	void load_texture(std::string texture_path);

	void unload_texture();

	void draw(gs_effect_t * effect);

private:
	gs_image_file_t * m_image = nullptr;

	uint16_t m_w, m_h;
	uint16_t texture_v_space;

	bool m_is_loaded = false;
	std::vector<Element> m_elements;

	uint16_t m_track_radius;
	uint16_t m_max_mouse_movement;
	float m_arrow_rot = 0.f;
};

};
#endif // LAYOUT_HPP
