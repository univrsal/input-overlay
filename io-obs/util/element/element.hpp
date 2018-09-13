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

extern "C" {
#include <graphics/image-file.h>
}

class ccl_config;

class element_data
{
public:
    element_data(const ElementType type)
    {
        m_type = type;
    }

    ElementType get_type() const
    {
        return m_type;
    }

    /* true if data should not me removed */
    virtual bool is_presistent() { return false; }

    /* used if is persistent */
    virtual void merge(element_data* other) { /* NO-OP */}
protected:
    ElementType m_type;
};

class element
{
public:
    virtual ~element() = default;

    element()
    {
        m_type = INVALID;
    }

    element(const ElementType type)
    {
        m_type = type;
    }

    virtual void load(ccl_config* cfg, const std::string& id) = 0;

    virtual void draw(gs_effect_t* effect, gs_image_file_t* m_image, element_data* data) = 0;

    ElementType get_type() const
    {
        return m_type;
    }

    uint16_t get_keycode() const
    {
        return m_keycode;
    }

protected:
    void read_size(ccl_config* cfg, const std::string& id);

    void read_pos(ccl_config* cfg, const std::string& id);

    void read_uv(ccl_config* cfg, const std::string& id);

    vec2 m_pos = {};
    gs_rect m_mapping = {};

    ElementType m_type = INVALID;
    uint16_t m_keycode = 0x0;
};
