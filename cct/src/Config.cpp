#include "Config.hpp"

namespace Overlay
{
	Config::Config(const std::string * texture, const std::string * config, SDL_helper * h)
	{

		m_texture_path = texture;
		m_config_path = config;
		m_helper = h;
		m_origin = m_helper->util_window_size();
		m_origin.x /= 2;
		m_origin.y /= 2;

	}

	void Config::draw_elements(void)
	{
		/* Draw origin cross (0/0) */
		SDL_Point window = m_helper->util_window_size();

		m_helper->util_draw_line(0, m_origin.y, window.x, m_origin.y, m_helper->palette()->white());
		m_helper->util_draw_line(m_origin.x, 0, m_origin.x, window.y, m_helper->palette()->white());

		/* Draw Scale*/

		for (int x = m_origin.x; x < window.x; x += (10 * m_scale_f))
		{
			bool flag = (x - m_origin.x) % (100 * m_scale_f) == 0 && (x - m_origin.x) != 0;

			if (flag)
			{
				std::string tag = std::to_string(x - m_origin.x);
				int w = m_helper->util_text_dim(&tag).w / 2;
				m_helper->util_text(&tag, x - w, m_origin.y - 20, m_helper->palette()->white());
				m_helper->util_draw_line(x, m_origin.y - 4, x, m_origin.y + 4, m_helper->palette()->white());
			}
			else
			{
				m_helper->util_draw_line(x, m_origin.y - 2, x, m_origin.y + 2, m_helper->palette()->white());	
			}
		}

		for (int y = m_origin.y; y < window.y; y += (10 * m_scale_f))
		{
			bool flag = (y - m_origin.y) % (100 * m_scale_f) == 0 && (y - m_origin.y) != 0;

			if (flag)
			{
				std::string tag = std::to_string(y - m_origin.y);
				SDL_Rect dim = m_helper->util_text_dim(&tag);
				
				m_helper->util_text(&tag, m_origin.x - dim.w - 5, y - dim.h /2, m_helper->palette()->white());
				m_helper->util_draw_line(m_origin.x - 4, y, m_origin.x + 4, y, m_helper->palette()->white());
			}
			else
			{
				m_helper->util_draw_line(m_origin.x - 2, y, m_origin.x + 2, y, m_helper->palette()->white());
			}
		}

		/* Draw elements */
	}

	void Config::handle_events(SDL_Event * e)
	{
		if (e->type == SDL_MOUSEBUTTONDOWN)
		{
			if (e->button.button == SDL_BUTTON_RIGHT)
			{
				m_dragging = true;
				m_drag_offset = { e->button.x - m_origin.x, e->button.y - m_origin.y };
			}
		}
		else if (e->type == SDL_MOUSEBUTTONUP)
		{
			if (e->button.button == SDL_BUTTON_RIGHT)
			{
				m_dragging = false;
			}
		}
		else if (e->type == SDL_MOUSEMOTION)
		{
			if (m_dragging)
			{
				m_origin.x = e->button.x - m_drag_offset.x;
				m_origin.y = e->button.y - m_drag_offset.y;
			}
		}
	}

	Element * Config::get_selected(void)
	{
		return m_selected;
	}

};
