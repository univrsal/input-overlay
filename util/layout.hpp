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

#include "../ccl/ccl.hpp"
#include "../hook/hook-helper.hpp"

/* Texture constants */
#define INNER_BORDER 3
#define OUTTER_BORDER 1

/* Config value names*/
#define DEFAULT_WIDTH "default_width"
#define DEFAULT_HEIGHT "default_height"

/**
 * This file is part of input-overlay
 * which is licenced under the MIT licence.
 * See LICENCE or https://mit-license.org
 * github.com/univrsal/input-overlay
 */

namespace Layout {

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

	enum ElementType
	{
		ELEMENT_TEXTURE,
		ELEMENT_BUTTON,
		ELEMENT_WHEEL,
		ELEMENT_MOUSE_MOVEMENT,
		ELEMENT_ANALOG_STICK,
		ELEMENT_TRIGGER,
		ELEMENT_TEXT
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
			: ElementData(ElementType::ELEMENT_BUTTON)
		{
			m_state = state;
		}

		ButtonState get_state() { return m_state; }
	private:
		ButtonState m_state;
	};

	class ElementDataWheel : public ElementData
	{
	public:
		ElementDataWheel(WheelDirection dir, int amount)
			: ElementData(ElementType::ELEMENT_WHEEL)
		{
			m_dir = dir;
			m_amount = amount;
		}

		int get_amount() { return m_amount; }
		WheelDirection get_dir() { return m_dir; }

	private:
		WheelDirection m_dir;
		int m_amount = 0;
	};

	class ElementDataHolder
	{
	public:
		void add_data(uint16_t keycode, ElementData * data);
		bool data_exists(uint16_t keycode);
		void remove_data(uint16_t keycode);
		ElementData * get_by_code(uint16_t keycode);

	private:
		std::map<uint16_t, std::unique_ptr<ElementData>> m_data;
	};

	class Element
	{
	public:
		Element(ElementType type)
		{
			m_type = type;
		}

		virtual void load(ccl_config * cfg, std::string id) = 0;
		virtual void draw(gs_effect_t *effect, gs_image_file_t * m_image, ElementData * data) = 0;

		ElementType get_type() { return m_type; }
		uint16_t get_keycode() { return m_keycode; }

	protected:
		void read_size(ccl_config * cfg, std::string id)
		{
			if (cfg->node_exists(id.append("_width")))
			{
				m_width = cfg->get_int(id.append("_width"));
				m_height = cfg->get_int(id.append("_height"));
			}
			else
			{
				m_width = cfg->get_int(DEFAULT_WIDTH);
				m_width = cfg->get_int(DEFAULT_HEIGHT);
			}
		}

		int m_xpos, m_ypos;
		int m_u, m_v;
		int m_width, m_height;
		ElementType m_type;
		uint16_t m_keycode;
	};

	class ElementTexture : public Element
	{
		void load(ccl_config * cfg, std::string id);
		void draw(gs_effect_t * effect, gs_image_file_t * image, ElementData * data);
	};

	class ElementButton : public Element
	{
		void load(ccl_config * cfg, std::string id);
		void draw(gs_effect_t * effect, gs_image_file_t * image, ElementData * data);
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
