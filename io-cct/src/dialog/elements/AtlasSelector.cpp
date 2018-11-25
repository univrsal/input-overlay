#include "AtlasSelector.hpp"

AtlasSelector::AtlasSelector(const int8_t id, const int x, const int y, const int w, const int h, Texture* t, Dialog* parent)
{
    const SDL_Rect temp = {x, y, w, h};
    AtlasSelector::init(parent, temp, id);
    m_atlas = t;
}

AtlasSelector::~AtlasSelector()
{
    AtlasSelector::close();
}

void AtlasSelector::close()
{
    GuiElement::close();

    delete m_cs;
    m_cs = nullptr;
}

void AtlasSelector::init(Dialog* parent, const SDL_Rect dim, const int8_t id)
{
    GuiElement::init(parent, dim, id);
    m_cs = new CoordinateSystem(SDL_Point{X_AXIS, Y_AXIS}, m_dimensions, get_helper());
    m_cs->enable_border();
    m_cs->enable_crosshair();
}

void AtlasSelector::draw_foreground()
{
    GuiElement::draw_foreground();

    auto temp = m_atlas->get_dim();
    temp.w *= m_cs->get_scale();
    temp.h *= m_cs->get_scale();
    temp.x = m_cs->get_origin_x();
    temp.y = m_cs->get_origin_y();

    m_cs->draw_foreground();

    m_cs->begin_draw();
    {
        m_atlas->draw(get_helper()->renderer(), &temp, nullptr);
    }
    m_cs->end_draw();

    m_cs->draw_selection();
}

void AtlasSelector::draw_background()
{
    m_cs->draw_background();
}

bool AtlasSelector::handle_events(SDL_Event* event, bool was_handled)
{
    auto handled = false;

    if (m_cs->handle_events(event))
        handled = true;

    return handled;
}

uint8_t AtlasSelector::get_cursor()
{
    return CURSOR_SPECIAL;
}

void AtlasSelector::resize()
{
    m_cs->set_pos(m_dimensions.x, m_dimensions.y);
    m_cs->set_dimensions(m_dimensions);
}
