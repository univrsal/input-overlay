/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "../../ccl/ccl.hpp"
#include "element_trigger.hpp"
#include "../util.hpp"

void element_trigger::load(ccl_config * cfg, const std::string & id)
{
    m_button_mode = cfg->get_bool(id + CFG_TRIGGER_MODE);
    m_side = (element_side) cfg->get_int(id + CFG_SIDE);

    if (!m_button_mode)
    {
        m_direction = (trigger_direction) cfg->get_int(id + CFG_DIRECTION);
    }
}

void element_trigger::draw(gs_effect_t * effect, gs_image_file_t * image, element_data * data)
{
    if (m_side == LEFT)
    {

    }
    else
    {

    }
}

void element_data_trigger::merge(element_data * other)
{
    if (other && other->get_type() == m_type)
    {
        element_data_trigger* trigger = reinterpret_cast<element_data_trigger*>(other);

        switch (trigger->m_data_type)
        {
        case BOTH:
            m_left_trigger = trigger->m_left_trigger;
            m_right_trigger = trigger->m_right_trigger;
            break;
        case LEFT:
            m_left_trigger = trigger->m_left_trigger;
            break;
        case RIGHT:
            m_right_trigger = trigger->m_right_trigger;
            break;
        default: ;
        }
    }
}
