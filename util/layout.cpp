#include "layout.hpp"

namespace Layout {

	Overlay::Overlay(std::string ini, std::string texture)
	{

	}

	void Overlay::load_cfg(std::string path)
	{
		ccl_config * cfg = new ccl_config(path, "");

		// TODO: Load general values and elements

		delete cfg;
		cfg = nullptr;
	}

	void Overlay::load_texture(std::string texture_path)
	{
		unload_texture();

		if (!texture_path.empty())
		{
			if (m_image == nullptr)
			{
				m_image = new gs_image_file_t();
			}

			gs_image_file_init(m_image, texture_path.c_str());

			obs_enter_graphics();
			gs_image_file_init_texture(m_image);
			obs_leave_graphics();

			if (!m_image->loaded)
			{
				blog(LOG_WARNING, "Error: failed to load texture %s", texture_path.c_str());
			}
		}
	}

	void Overlay::unload_texture()
	{
		obs_enter_graphics();
		gs_image_file_free(m_image);
		obs_leave_graphics();
	}

	void Overlay::draw(gs_effect_t * effect)
	{
		for (auto element : m_elements)
		{
			ElementData * data = nullptr;
			if (element.get_type() != ELEMENT_TEXTURE)
			{
				data = Hook::input_data->get_by_code(element.get_keycode());
			}
			element.draw(effect, m_image, data);
		}
	}

	void ElementTexture::load(ccl_config * cfg, std::string id)
	{
		m_xpos = cfg->get_int(id.append("_x_pos"));
		m_ypos = cfg->get_int(id.append("_y_pos"));

		m_u = cfg->get_int(id.append("_u"));
		m_v = cfg->get_int(id.append("_v"));

		read_size(cfg, id);
	}

	void ElementTexture::draw(gs_effect_t * effect, gs_image_file_t * image, ElementData * data)
	{
		gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"), image->texture);
		gs_matrix_translate3f((float)m_xpos, (float)m_ypos, 1.f);

		gs_draw_sprite_subregion(image->texture, 0, m_u, m_v + m_height + INNER_BORDER,
			m_width + OUTTER_BORDER, m_height + OUTTER_BORDER);
	}

	void ElementButton::load(ccl_config * cfg, std::string id)
	{
		m_xpos = cfg->get_int(id.append("_x_pos"));
		m_ypos = cfg->get_int(id.append("_y_pos"));

		m_u = cfg->get_int(id.append("_u"));
		m_v = cfg->get_int(id.append("_v"));

		read_size(cfg, id);

		m_keycode = cfg->get_hex(id.append("_code"));
	}

	void ElementButton::draw(gs_effect_t * effect, gs_image_file_t * image, ElementData * data)
	{

	}

	void ElementDataHolder::add_data(uint16_t keycode, ElementData * data)
	{
		remove_data(keycode);
		m_data[keycode] = std::unique_ptr<ElementData>(data);
	}

	bool ElementDataHolder::data_exists(uint16_t keycode)
	{
		return m_data[keycode] != nullptr;
	}

	void ElementDataHolder::remove_data(uint16_t keycode)
	{
		if (data_exists(keycode))
			m_data[keycode].reset(nullptr);
	}

	ElementData * ElementDataHolder::get_by_code(uint16_t keycode)
	{
		if (data_exists(keycode))
			return m_data[keycode].get();
		return nullptr;
	}

};
