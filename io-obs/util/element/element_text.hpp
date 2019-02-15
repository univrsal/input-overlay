/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include "element_texture.hpp"

class element_text : public element
{
public:
    element_text();

    void load(ccl_config* cfg, const std::string& id) override;

    void draw(gs_effect_t* effect, gs_image_file_t* image,
        element_data* data, sources::shared_settings* settings) override;

    data_source get_source() override;

private:
    bool m_use_total_values = false;
    std::string m_unformatted_text;
    
    void format(std::string& str, const char* find, int replace) const;
};

