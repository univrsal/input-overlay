/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "../../sources/input_source.hpp"
#include "../../../ccl/ccl.hpp"
#include "element_dpad.hpp"
#include "../util.hpp"

void element_dpad::load(ccl_config* cfg, const std::string& id)
{

}

void element_dpad::draw(gs_effect_t* effect,
    gs_image_file_t* image, element_data* data, sources::shared_settings* settings)
{
    
    element_texture::draw(effect, image, nullptr);
}

void element_data_dpad::merge(element_data* other)
{
    auto d = dynamic_cast<element_data_dpad*>(other);
    if (d)
    {
        switch(d->m_direction)
        {
            case DPAD_LEFT:
                switch(m_direction)
                {
                    case DPAD_UP:
                        m_direction = DPAD_TOP_LEFT;
                        break;
                    case DPAD_DOWN:
                        m_direction = DPAD_BOTTOM_LEFT;
                        break;
                    default:
                        m_direction = d->m_direction;
                }
                break;
            case DPAD_RIGHT:
                switch(m_direction)
                {
                    case DPAD_UP:
                        m_direction = DPAD_TOP_RIGHT ;
                        break;
                    case DPAD_DOWN:
                        m_direction = DPAD_BOTTOM_RIGHT;
                        break;
                    default:
                        m_direction = d->m_direction;
                }
                break;
            case DPAD_UP:
                switch(m_direction)
                {
                    case DPAD_LEFT:
                        m_direction = DPAD_TOP_LEFT ;
                        break;
                    case DPAD_RIGHT:
                        m_direction = DPAD_TOP_RIGHT;
                        break;
                    default:
                        m_direction = d->m_direction;
                }
                break;
            case DPAD_DOWN:
                switch(m_direction)
                {
                    case DPAD_LEFT:
                        m_direction = DPAD_BOTTOM_LEFT ;
                        break;
                    case DPAD_RIGHT:
                        m_direction = DPAD_BOTTOM_RIGHT;
                        break;
                    default:
                        m_direction = d->m_direction;
                }
                break;
            case DPAD_TOP_LEFT:break;
            case DPAD_TOP_RIGHT:break;
            case DPAD_BOTTOM_LEFT:break;
            case DPAD_BOTTOM_RIGHT:break;
        }
        
    }
}
