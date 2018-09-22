/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */
#pragma once

#include "../layout_constants.hpp"
#include <string>
#include "graphics/vec2.h"
#include "../util.hpp"

extern "C" {
#include <graphics/image-file.h>
}

/**
 * Which data holder to read element
 * data from
 */
enum data_source
{
    NONE = -1,
    DEFAULT,
    GAMEPAD
};

class ccl_config;

class element_data
{
public:
    element_data(const element_type type)
    {
        m_type = type;
    }

    element_type get_type() const
    {
        return m_type;
    }

    /* true if data should not me removed */
    virtual bool is_presistent() { return false; }

    /* used if is persistent */
    virtual void merge(element_data* other)
    {
        /* NO-OP */
    }

protected:
    element_type m_type;
};

class element
{
public:
    virtual ~element() = default;

    element()
    {
        m_type = INVALID;
    }

    element(const element_type type)
    {
        m_type = type;
    }

    virtual void load(ccl_config* cfg, const std::string& id) = 0;

    virtual void draw(gs_effect_t* effect, gs_image_file_t* m_image,
                      element_data* data) = 0;

    element_type get_type() const
    {
        return m_type;
    }

    uint16_t get_keycode() const
    {
        return m_keycode;
    }

     virtual data_source get_source() { return DEFAULT; }
protected:
    void read_mapping(ccl_config* cfg, const std::string& id);

    void read_pos(ccl_config* cfg, const std::string& id);

    vec2 m_pos = {};
    gs_rect m_mapping = {};

    element_type m_type = INVALID;
    uint16_t m_keycode = VC_NONE;
};
