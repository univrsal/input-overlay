#include "Config.hpp"

namespace Overlay
{
	Config::Config(const std::string * texture, const std::string * config, SDL_helper * h)
	{

		m_texture_path = texture;
		m_config_path = config;
		m_helper = h;
		m_origin.x = 90;
		m_origin.y = 70;

	}

	void Config::draw_elements(void)
	{
		/* Draw origin cross (0/0) */
		SDL_Point window = m_helper->util_window_size();

		m_helper->util_draw_line(0, m_origin.y, window.x, m_origin.y, m_helper->palette()->white());
		m_helper->util_draw_line(m_origin.x, 0, m_origin.x, window.y, m_helper->palette()->white());

		
		/* Axe titles */
		std::string t = "X in pixels";
		SDL_Rect dim = m_helper->util_text_dim(&t);

		m_helper->util_text(&t, m_origin.x - dim.w - 5, m_origin.y - dim.h, m_helper->palette()->white());
		t = "Y in pixels";
		dim = m_helper->util_text_dim(&t);
		m_helper->util_text(&t, m_origin.x - 5, m_origin.y + 5, m_helper->palette()->white(), 90);

		t = "Scale: " + std::to_string(m_scale_f);
		dim = m_helper->util_text_dim(&t);
		m_helper->util_text(&t, window.x - dim.w - 5, 5, m_helper->palette()->white());

		/* Draw Scale*/
		for (int x = m_origin.x; x < window.x; x += (10 * m_scale_f))
		{
			bool flag = (x - m_origin.x) % (100 * m_scale_f) == 0 && (x - m_origin.x) != 0;

			if (flag)
			{
				std::string tag = std::to_string(x - m_origin.x);
				SDL_Rect dim = m_helper->util_text_dim(&tag);
				
				m_helper->util_text(&tag, x + dim.h / 2, m_origin.y - dim.w - 6, m_helper->palette()->white(), 90);
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
				m_dragging_all = true;
				m_drag_offset = { e->button.x - m_origin.x, e->button.y - m_origin.y };
			}
			else if (e->button.button == SDL_BUTTON_LEFT)
			{
				/* Handle selection of elements */
				std::vector<std::unique_ptr<Element>>::iterator iterator;

				for (iterator = m_elements.begin(); iterator != m_elements.end(); iterator++)
				{
					if (m_helper->util_is_in_rect(iterator->get()->get_dim(), e->button.x, e->button.y))
					{
						m_selected = iterator->get();
						m_dragging_element = true;
						break;
					}
				}
			}
		}
		else if (e->type == SDL_MOUSEBUTTONUP)
		{
			if (e->button.button == SDL_BUTTON_RIGHT)
			{
				m_dragging_all = false;
			}
			else if (e->button.button == SDL_BUTTON_LEFT)
			{
				m_dragging_element = false;
			}
		}
		else if (e->type == SDL_MOUSEMOTION)
		{
			if (m_dragging_all)
			{
				m_origin.x = SDL_min(e->button.x - m_drag_offset.x, 90);
				m_origin.y = SDL_min(e->button.y - m_drag_offset.y, 70);
			}

			if (m_dragging_element && m_selected)
			{
				int x, y;
				x = (e->button.x - m_origin.x) / m_scale_f;
				y = (e->button.y - m_origin.y) / m_scale_f;
				m_selected->set_pos(x, y);
			}
		}
		else if (e->type == SDL_MOUSEWHEEL)
		{
			if (e->wheel.y > 0) /* UP */
			{
				m_scale_f = SDL_min(m_scale_f++, 7);
			}
			else
			{
				m_scale_f = SDL_max(m_scale_f--, 1);
			}
		}
	}
};
