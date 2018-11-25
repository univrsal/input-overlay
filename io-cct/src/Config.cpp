/**
 * Created by universal on 27.05.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "Config.hpp"
#include "dialog/DialogElementSettings.hpp"
#include "util/Notifier.hpp"
#include "util/SDL_Helper.hpp"
#include "util/Constants.hpp"
#include "util/Texture.hpp"
#include "../../ccl/ccl.hpp"
#include "element/ElementAnalogStick.hpp"



Config::Config(const char* texture, const char* config, const SDL_Point def_dim, const SDL_Point space, SDL_Helper* h,
               DialogElementSettings* s)
{
    m_texture_path = texture;
    m_config_path = config;

    m_settings = s;
    m_atlas = new Texture(texture, h->renderer());
    m_helper = h;
    m_default_dim = def_dim;
    m_offset = space;

    const auto w = h->util_window_size();
    m_cs = CoordinateSystem(SDL_Point{X_AXIS, Y_AXIS}, SDL_Rect{0, 0, w->x, w->y}, h);

    if (def_dim.x > 0 && def_dim.y > 0)
        m_cs.set_grid_space(m_default_dim);
    if (space.x != 0 && space.y != 0)
        m_cs.set_ruler_offset(space);
}

Config::~Config()
{
    delete m_atlas;
    m_atlas = nullptr;
    m_helper = nullptr;
    m_settings = nullptr;
}

void Config::draw_elements()
{
    /* Draw coordinate system */
    m_cs.draw_background();
    m_cs.draw_foreground();

    /* Draw elements */
    m_cs.begin_draw();
    {
        auto elements_drawn = 0;
        auto layer = 0;

        while (elements_drawn < m_elements.size())
        {
            for (auto const& element : m_elements)
            {
                if (element->get_z_level() == layer)
                {
                    element->draw(m_atlas, &m_cs, element.get() == m_selected, m_helper->is_ctrl_down());
                    elements_drawn++;
                }
            }
            layer++;
        }

        if (!SDL_RectEmpty(&m_total_selection))
        {
            auto temp = m_total_selection;

            temp.x = temp.x * m_cs.get_scale() + m_cs.get_origin_x();
            temp.y = temp.y * m_cs.get_scale() + m_cs.get_origin_y();
            temp.w *= m_cs.get_scale();
            temp.h *= m_cs.get_scale();

            m_helper->util_draw_rect(&temp, m_helper->palette()->red());
        }

        if (!SDL_RectEmpty(&m_temp_selection))
        {
            auto temp = m_temp_selection;
            temp.x = temp.x * m_cs.get_scale() + m_cs.get_origin_x();
            temp.y = temp.y * m_cs.get_scale() + m_cs.get_origin_y();
            temp.w *= m_cs.get_scale();
            temp.h *= m_cs.get_scale();
            m_helper->util_draw_rect(&temp, m_helper->palette()->white());
        }

        if (m_element_to_delete >= 0 && m_element_to_delete < m_elements.size())
        {
            m_elements.erase(m_elements.begin() + m_element_to_delete);
            if (m_element_to_delete == m_selected_id)
            {
                m_selected = nullptr;
                m_selected_id = -1;
            }
            m_element_to_delete = -1;
        }
    }
    m_cs.end_draw();
}

void Config::handle_events(SDL_Event* e)
{
    m_cs.handle_events(e);
    auto m_x = e->button.x, m_y = e->button.y;

    if (e->type == SDL_MOUSEBUTTONDOWN)
    {
        if (e->button.button == SDL_BUTTON_LEFT)
        {
            /* Handle selection of elements */
            m_cs.translate(m_x, m_y);

            if (SDL_RectEmpty(&m_total_selection))
                /* No multiple items already selected */
            {
                auto i = 0;
                auto highest_layer = 0;
                m_selected_id = -1;

                for (auto const& elem : m_elements)
                {
                    if (SDL_Helper::util_is_in_rect(elem->get_abs_dim(&m_cs), m_x, m_y))
                    {
                        if (elem->get_z_level() >= highest_layer)
                        {
                            highest_layer = elem->get_z_level();
                            m_selected = m_elements[i].get();
                            m_selected_id = i;
                        }
                        m_in_single_selection = true;
                    }
                    i++;
                }

                if (m_in_single_selection) /* Start single element selection */
                {
                    m_drag_offset = {
                        (e->button.x - (m_selected->get_x() * m_cs.get_scale())
                            + m_cs.get_origin()->x),
                        (e->button.y - (m_selected->get_y() * m_cs.get_scale())
                            + m_cs.get_origin()->y)
                    };
                    m_settings->select_element(m_selected);
                }
                else /* Start multi element selection */
                {
                    m_in_multi_selection = true;
                    reset_selection();
                    m_selection_start = {e->button.x, e->button.y};
                }
            }
            else if (SDL_Helper::util_is_in_rect(&m_total_selection, e->button.x, e->button.y))
            {
                m_dragging_elements = true;
                m_drag_offset = {
                    e->button.x - m_total_selection.x,
                    e->button.y - m_total_selection.y
                };
            }
            else
            {
                m_total_selection = {};
            }
        }
    }
    else if (e->type == SDL_MOUSEBUTTONUP)
    {
        m_in_single_selection = false;
        m_dragging_elements = false;
        m_temp_selection = {};
        m_in_multi_selection = false;
    }
    else if (e->type == SDL_MOUSEMOTION)
    {
        if (m_in_single_selection && m_selected)
            /* Dragging single element */
        {
            move_element(e->button.x, e->button.y);
        }
        else if (m_in_multi_selection)
            /* Selecting multiple elements */
        {
            SDL_Rect elem_dim;
            SDL_Rect elem_abs_dim;

            m_temp_selection.x = UTIL_MIN(e->button.x, m_selection_start.x);
            m_temp_selection.y = UTIL_MIN(e->button.y, m_selection_start.y);

            m_cs.translate(m_temp_selection.x, m_temp_selection.y);

            m_temp_selection.x = ceil(UTIL_MAX((m_temp_selection.x - m_cs.get_origin_x()) /
                ((float)m_cs.get_scale()), 0));
            m_temp_selection.y = ceil(UTIL_MAX((m_temp_selection.y - m_cs.get_origin_y()) /
                ((float)m_cs.get_scale()), 0));

            m_temp_selection.w = ceil(SDL_abs(m_selection_start.x - e->button.x)
                / static_cast<float>(m_cs.get_scale()));
            m_temp_selection.h = ceil(SDL_abs(m_selection_start.y - e->button.y)
                / static_cast<float>(m_cs.get_scale()));

            m_selected_elements.clear();

            uint16_t index = 0;
            for (auto const& elem : m_elements)
            {
                if (is_rect_in_rect(elem->get_mapping(), &m_temp_selection))
                {
                    m_cs.translate(elem_abs_dim.x, elem_abs_dim.y);
                    m_selected_elements.emplace_back(index);
                    SDL_UnionRect(&m_total_selection, &elem_dim, &m_total_selection);
                }
                index++;
            }
        }
        else if (m_dragging_elements)
            /* Dragging multiple elements */
        {
            move_elements(e->button.x - m_drag_offset.x,
                          e->button.y - m_drag_offset.y);
        }
    }
    else if (e->type == SDL_KEYDOWN)
    {
        if (m_selected)
            /* Move selected element with arrow keys */
        {
            auto x = m_selected->get_x();
            auto y = m_selected->get_y();

            if (util_move_element(&x, &y, e->key.keysym.sym))
            {
                m_selected->set_pos(x, y);
                m_settings->set_xy(x, y);
            }
        }
        else if (!m_selected_elements.empty())
        {
            auto x = m_total_selection.x, y = m_total_selection.y;

            if (util_move_element(&x, &y, e->key.keysym.sym))
                move_elements(x, y);
        }
    }
    else if (e->type == SDL_WINDOWEVENT
        && e->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
    {
        const auto w = m_helper->util_window_size();
        m_cs.set_dimensions(SDL_Rect{0, 0, w->x, w->y});
    }

    /* Let elements react to SDL events */
    for (auto& element : m_elements)
        element->handle_event(e, m_helper);
}

void Config::write_config(Notifier* n)
{
    if (m_elements.empty())
    {
        n->add_msg(MESSAGE_INFO, m_helper->loc(LANG_MSG_NOTHING_TO_SAVE));
        return;
    }

    const auto start = SDL_GetTicks();
    auto cfg = ccl_config(m_config_path, "CCT generated config");

    cfg.free_nodes(); /* Don't need existing values */

    cfg.add_string(CFG_FIRST_ID, "Starting point for loading elements", *m_elements[0]->get_id(), true);
    cfg.add_int(CFG_DEFAULT_WIDTH, "", m_default_dim.x);
    cfg.add_int(CFG_DEFAULT_HEIGHT, "Default element dimension", m_default_dim.y);
    cfg.add_int(CFG_H_SPACE, "", m_offset.x);
    cfg.add_int(CFG_V_SPACE, "Element offset for visual help", m_offset.y);

    auto height = 0, width = 0, index = 0; /* The most bottom right element determines the width/height */

    uint8_t flags = 0; /* Determines which properties to show in OBS */
    for (auto const& e : m_elements)
    {
        e->write_to_file(&cfg, &m_default_dim, flags);

        width = UTIL_MAX(width, e->get_x() + e->get_w());
        height = UTIL_MAX(height, e->get_y() + e->get_h());

        if (index + 1 < m_elements.size())
        {
            cfg.add_string((*e->get_id()) + CFG_NEXT_ID, "Next element in list",
                           *m_elements[index + 1]->get_id());
        }
        index++;
    }

    cfg.add_int(CFG_TOTAL_WIDTH, "", width);
    cfg.add_int(CFG_TOTAL_HEIGHT, "Full overlay dimensions", height);

    cfg.add_int(CFG_FLAGS, "", flags);

    cfg.write(false);

    const auto end = SDL_GetTicks();

    if (cfg.has_fatal_errors())
    {
        n->add_msg(MESSAGE_ERROR, m_helper->loc(LANG_MSG_SAVE_ERROR));
        n->add_msg(MESSAGE_ERROR, cfg.get_error_message());
    }
    else
    {
        const auto result = SDL_Helper::format(m_helper->loc(LANG_MSG_SAVE_SUCCESS).c_str(), m_elements.size(),
                                              (end - start));
        n->add_msg(MESSAGE_INFO, result);
    }
    cfg.free_nodes();
}

void Config::read_config(Notifier* n)
{
    const auto start = SDL_GetTicks();
    auto cfg = ccl_config(m_config_path, "");

    if (cfg.is_empty())
    {
        n->add_msg(MESSAGE_INFO, m_helper->loc(LANG_MSG_CONFIG_EMPTY));
        return;
    }

    if (!cfg.node_exists(CFG_FIRST_ID))
    {
        n->add_msg(MESSAGE_INFO, m_helper->loc(LANG_MSG_CONFIG_CORRUPT));
        return;
    }

    auto element_id = cfg.get_string(CFG_FIRST_ID);

    if (cfg.get_int(CFG_DEFAULT_WIDTH) != 0)
        m_default_dim.x = cfg.get_int(CFG_DEFAULT_WIDTH);
    if (cfg.get_int(CFG_DEFAULT_HEIGHT) != 0)
        m_default_dim.y = cfg.get_int(CFG_DEFAULT_HEIGHT);

    auto type = 0;

    while (!element_id.empty())
    {
        type = cfg.get_int(element_id + CFG_TYPE);
        if (!Element::valid_type(type))
        {
            n->add_msg(MESSAGE_ERROR, m_helper->format_loc(
                           LANG_MSG_VALUE_TYPE_INVALID, element_id.c_str(), type));

            element_id = cfg.get_string(element_id.append(CFG_NEXT_ID));
            continue;
        }

        auto e = Element::read_from_file(&cfg, element_id,
                                             static_cast<element_type>(type), &m_default_dim);
        if (e)
            m_elements.emplace_back(e);
        else
            n->add_msg(MESSAGE_ERROR, m_helper->format_loc(
                           LANG_MSG_ELEMENT_LOAD_ERROR, element_id.c_str()));
        element_id = cfg.get_string(element_id.append(CFG_NEXT_ID));
    }

    const auto end = SDL_GetTicks();

    if (cfg.has_fatal_errors())
    {
        n->add_msg(MESSAGE_ERROR, m_helper->loc(LANG_MSG_LOAD_ERROR));
        n->add_msg(MESSAGE_ERROR, cfg.get_error_message());
    }
    else
    {
        const auto result = SDL_Helper::format(m_helper->
                                              loc(LANG_MSG_LOAD_SUCCESS).c_str(), m_elements.size(), (end - start));
        n->add_msg(MESSAGE_INFO, result);
    }
}

Texture* Config::get_texture() const
{
    return m_atlas;
}

SDL_Point Config::get_default_dim() const
{
    return m_default_dim;
}

void Config::reset_selection()
{
    m_selected = nullptr;
    m_settings->set_id("");
    m_settings->set_uv(0, 0);
    m_settings->set_xy(0, 0);
    m_settings->set_wh(0, 0);
    m_settings->set_vc(0);
    m_selected_elements.clear();
    m_total_selection = {};
}

void Config::move_elements(const int new_x, const int new_y)
{
    if (!m_selected_elements.empty())
    {
        const auto delta_x = new_x - m_total_selection.x;
        const auto delta_y = new_y - m_total_selection.y;

        const auto flag_x = new_x >= 0, flag_y = new_y >= 0;

        if (flag_x)
            m_total_selection.x = new_x;
        if (flag_y)
            m_total_selection.y = new_y;

        if (flag_x || flag_y)
            for (auto const& index : m_selected_elements)
            {
                if (index < m_elements.size())
                {
                    auto e = m_elements[index].get();
                    if (e)
                    {
                        e->set_pos(
                            e->get_x() + (flag_x ? delta_x : 0),
                            e->get_y() + (flag_y ? delta_y : 0));
                    }
                }
            }
    }
}

inline void Config::move_element(const int mouse_x, const int mouse_y)
{
    const auto x = SDL_max((mouse_x - m_drag_offset.x +
        m_cs.get_origin()->x) / m_cs.get_scale(), 0);
    const auto y = SDL_max((mouse_y - m_drag_offset.y +
        m_cs.get_origin()->y) / m_cs.get_scale(), 0);

    m_selected->set_pos(x, y);
    m_settings->set_xy(x, y);
}

inline bool Config::is_rect_in_rect(const SDL_Rect* a, const SDL_Rect* b)
{
    return a->x >= b->x && a->x + a->w <= b->x + b->w
        && a->y >= b->y && a->y + a->h <= b->y + b->h;
}
