#ifndef LAYOUT_HPP
#define LAYOUT_HPP

#ifdef LINUX
#include <stdint.h>
#endif
#include <vector>
#include <obs-module.h>
#include <string.h>
#include <map>

extern "C" {
#include <graphics/image-file.h>
}

#include "../../ccl/ccl.hpp"
#include "../hook/hook-helper.hpp"
#include "layout_constants.hpp"

/* Texture constants */
#define INNER_BORDER 3
#define OUTTER_BORDER 1
#define PRESSED_OFFSET 2

/**
 * This file is part of input-overlay
 * which is licensed under the MIT license.
 * See LICENCE or https://mit-license.org
 * github.com/univrsal/input-overlay
 */

 //namespace Data {

enum ButtonState
{
	STATE_PRESSED,
	STATE_RELEASED
};

enum WheelDirection
{
	WHEEL_DIR_UP = -1,
	WHEEL_DIR_NONE = 0,
	WHEEL_DIR_DOWN = 1
};

class ElementData
{
public:
	ElementData(ElementType type)
	{
		m_type = type;
	}

	ElementType get_type()
	{
		return m_type;
	}
protected:
	ElementType m_type;
};

class ElementDataButton : public ElementData
{
public:
	ElementDataButton(ButtonState state)
		: ElementData(ElementType::BUTTON)
	{
		m_state = state;
	}

	ButtonState get_state()
	{
		return m_state;
	}
private:
	ButtonState m_state;
};

class ElementDataWheel : public ElementData
{
public:
	ElementDataWheel(WheelDirection dir, int amount)
		: ElementData(ElementType::MOUSE_SCROLLWHEEL)
	{
		m_dir = dir;
		m_amount = amount;
	}

	int get_amount()
	{
		return m_amount;
	}
	WheelDirection get_dir()
	{
		return m_dir;
	}

private:
	WheelDirection m_dir;
	int m_amount = 0;
};

class ElementDataHolder
{
public:
	~ElementDataHolder()
	{
		m_data.clear();
		m_map_cleared = true;
	}
	void add_data(uint16_t keycode, ElementData * data);
	bool data_exists(uint16_t keycode);
	void remove_data(uint16_t keycode);
	ElementData * get_by_code(uint16_t keycode);
	bool is_empty(void)
	{
		return m_map_cleared || m_data.empty();
	}

	bool m_map_cleared = false;
	std::map<uint16_t, std::unique_ptr<ElementData>> m_data;
};

class Element
{
public:
	Element(ElementType type)
	{
		m_type = type;
	}

	virtual void load(ccl_config * cfg, std::string id)
	{ /*NO-OP*/
	};
	virtual void draw(gs_effect_t * effect, gs_image_file_t * m_image, ElementData * data)
	{ /*NO-OP*/
	};

	ElementType get_type()
	{
		return m_type;
	}
	uint16_t get_keycode()
	{
		return m_keycode;
	}

protected:
	void read_size(ccl_config * cfg, std::string id)
	{
		if (cfg->node_exists(id + CFG_WIDTH))
			m_width = cfg->get_int(id + CFG_WIDTH);
		else
			m_width = cfg->get_int(CFG_DEFAULT_WIDTH);

		if (cfg->node_exists(id + CFG_HEIGHT))
			m_height = cfg->get_int(id + CFG_HEIGHT);
		else
			m_height = cfg->get_int(CFG_DEFAULT_HEIGHT);
	}

	void read_pos(ccl_config * cfg, std::string id)
	{
		m_xpos = cfg->get_int(id + CFG_X_POS);
		m_ypos = cfg->get_int(id + CFG_Y_POS);
	}

	void read_uv(ccl_config * cfg, std::string id)
	{
		m_u = cfg->get_int(id + CFG_U);
		m_v = cfg->get_int(id + CFG_V);
	}

	int m_xpos, m_ypos;
	int m_u, m_v;
	int m_width, m_height;
	ElementType m_type;
	uint16_t m_keycode;
};

class ElementTexture : public Element
{
public:
	ElementTexture() : Element(ElementType::TEXTURE) { };
	void load(ccl_config * cfg, std::string id) override;
	void draw(gs_effect_t * effect, gs_image_file_t * image, ElementData * data) override;
};

class ElementButton : public Element
{
public:
	ElementButton() : Element(ElementType::BUTTON) { };
	void load(ccl_config * cfg, std::string id) override;
	void draw(gs_effect_t * effect, gs_image_file_t * image, ElementData * data) override;
};

class Overlay
{
public:
	Overlay()
	{ /* NO-OP*/
	}
	Overlay(std::string ini, std::string texture);

	bool load(std::string cfg_path, std::string texture_path);
	void unload();

	void draw(gs_effect_t * effect);
	bool is_loaded()
	{
		return m_is_loaded;
	}
	gs_image_file_t * get_texture()
	{
		return m_image;
	}

	int get_widt() { return m_w; }
	int get_height() { return m_h; }
private:
	bool load_cfg(std::string path);
	bool load_texture(std::string path);
	void unload_texture();
	void load_element(ccl_config * cfg, std::string id);

	gs_image_file_t * m_image = nullptr;

	uint16_t m_w, m_h;
	uint16_t m_default_element_w, m_default_element_h;
	uint16_t texture_v_space;

	bool m_is_loaded = false;
	std::vector<std::unique_ptr<Element>> m_elements;

	uint16_t m_track_radius;
	uint16_t m_max_mouse_movement;
	float m_arrow_rot = 0.f;
};

#endif // LAYOUT_HPP
