/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "input_entry.hpp"
#include "../../sources/input_history.hpp"
#include "effect.hpp"
#include "../element/element_data_holder.hpp"

/* Maps a specific mask value to a keycode
 * libuiohook repports events with masks
 * which can be used to detect key combinations
 * e.g. when the users presses Ctr+C, the event
 * will have the keycode C with the mask MASK_CTRL_L
 * (if the left control key was used)
 * Since input-history translates keycodes into text
 * this maps each mask to the respective keycode
 * for easier translation
 */
struct mask_vc
{
    uint16_t mask, vc;
};

const std::string plus = " + ";
mask_vc mask_vc_map[] = { {MASK_ALT_L, VC_ALT_L}, {MASK_ALT_R, VC_ALT_R},
    {MASK_CTRL_L, VC_CONTROL_L}, {MASK_CTRL_R, VC_CONTROL_R},
    {MASK_SHIFT_L, VC_SHIFT_L}, {MASK_SHIFT_R, VC_SHIFT_R},
    {MASK_META_L, VC_META_L}, {MASK_META_R, VC_META_R},
    {MASK_CAPS_LOCK, VC_CAPS_LOCK}, {MASK_NUM_LOCK, VC_NUM_LOCK},
    {MASK_SCROLL_LOCK, VC_SCROLL_LOCK}, {MASK_BUTTON1, VC_MOUSE_BUTTON1},
    {MASK_BUTTON2, VC_MOUSE_BUTTON2}, {MASK_BUTTON3, VC_MOUSE_BUTTON3},
    {MASK_BUTTON4, VC_MOUSE_BUTTON4}, {MASK_BUTTON5, VC_MOUSE_BUTTON5}};

void input_entry::mask_to_string(std::string& str, uint16_t mask, bool use_fallback, key_names* names)
{
    const char* name = nullptr;
    

    for (const auto& m : mask_vc_map)
    {
        if (mask & m.mask)
        {
            if ((name = names->get_name(m.vc)))
                str += name + plus;
            else if (use_fallback)
                str += key_to_text(m.vc) + plus;
        }
    }
}

void input_entry::collect_inputs(element_data_holder* data)
{
    if (data)
        data->populate_vector(m_inputs, 0);
}

void input_entry::build_string(key_names* names, sources::history_settings* settings)
{
    const bool use_fallback = settings->flags & sources::FLAG_USE_FALLBACK;
    const char* name = nullptr;

    if (settings->flags & sources::FLAG_USE_MASKS)
    { 
        mask_to_string(m_text, m_mask, use_fallback, names);
    }

    for (const auto& key : m_inputs)
    {
        if ((name = names->get_name(key)))
            m_text += name + plus;
        else if (use_fallback)
            m_text += key_to_text(key) + plus;
    }

    /* Remove the last ' + '*/
    m_text.erase(m_text.length() - 3);
}

void input_entry::tick(const float seconds)
{
    for (auto& effect : m_effects)
        effect->tick(seconds);
}

void input_entry::add_effect(effect* e)
{
    m_effects.emplace_back(e);
}

void input_entry::render_text(obs_source_t* text_source, sources::history_settings* settings)
{
    gs_matrix_push();
    

    for (auto& effect : m_effects)
        effect->render();
    gs_matrix_pop();
}

void input_entry::render_icons(sources::history_settings* settings)
{
    gs_matrix_push();

    for (auto& effect : m_effects)
        effect->render();
    gs_matrix_pop();
}
