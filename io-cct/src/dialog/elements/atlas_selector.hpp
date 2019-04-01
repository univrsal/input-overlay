/**
 * Created by universal on 02.05.2018.
 * This file is part of reloded which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "../../util/coordinate_system.hpp"
#include "../../util/sdl_helper.hpp"
#include "../../util/texture.hpp"
#include <SDL.h>
#include "gui_element.hpp"
#include "../dialog.hpp"

class sdl_helper;

class coordinate_system;

class texture;

class atlas_selector : public gui_element
{
public:
    atlas_selector(int8_t id, int x, int y, int w, int h, texture* t, dialog* parent);

    ~atlas_selector();

    void close() override;

    void init(dialog* parent, SDL_Rect dim, int8_t id) override;

    void draw_foreground() override;

    void draw_background() override;

    bool handle_events(SDL_Event* event, bool was_handled) override;

    bool selection_changing() const
    { return m_cs->changing_selection(); }

    uint8_t get_cursor() override;

    void set_selection(SDL_Rect* r) const
    { m_cs->set_selection(r); }

private:
    void resize() override;

    texture* m_atlas; /* Is loaded in config, and will be deleted there */
    coordinate_system* m_cs{};
};
