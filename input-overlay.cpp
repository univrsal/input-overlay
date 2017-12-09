#include <obs-module.h>
extern "C" {
#include <graphics\image-file.h>
#include "util.h"
}
#include <graphics\vec2.h>
#include <string>
#include <vector>
#include <algorithm>
#include <Windows.h>
#include <iostream>
#include <fstream>

using namespace std;
#define SHIFTED 0x8000 

#define KEY_LMB     VK_LBUTTON
#define KEY_RMB     VK_RBUTTON
#define KEY_MMB     VK_MBUTTON
#define KEY_SMB1    VK_XBUTTON1
#define KEY_SMB2    VK_XBUTTON2
#define KEY_NONE    0x07

#define MIN_SIZE_CX 2
#define MIN_SIZE_CY 2
#define MAX_SIZE_CX 16384
#define MAX_SIZE_CY 16384

#define MAX_AREA (4096LL * 4096LL)

#define warning(format, ...) blog(LOG_WARNING, "[%s] " format, \
		obs_source_get_name(m_source), ##__VA_ARGS__)
// Lang
#define S_OVERLAY_FILE              "overlay_image"
#define S_LAYOUT_FILE               "layout_file"

#define T_(v)                       obs_module_text(v)
#define T_OVERLAY_FILE              T_("OverlayFile")
#define T_LAYOUT_FILE               T_("LayoutFile")
#define T_FILTER_IMAGE_FILES        T_("Filter.ImageFiles")
#define T_FILTER_TEXT_FILES         T_("Filter.TextFiles")
#define T_FILTER_ALL_FILES          T_("Filter.AllFiles")

// Util structs
struct InputKey {
    unsigned char m_key_char;
    bool m_pressed;
    uint16_t texture_u, texture_v;
    uint16_t w, h;

    uint16_t x_offset; // used to center buttons that span over multiple columns
    uint16_t row; // On screen location
    uint16_t column;
};

struct OverlayLayout {
    bool m_is_keyboard = true;
    uint8_t m_key_count;
    int8_t m_key_space_v, m_key_space_h;
    uint8_t m_btn_w, m_btn_h;
    uint16_t m_w, m_h;
    uint8_t m_rows, m_cols;
    uint8_t texture_w;
    uint16_t texture_v_space;
    bool m_is_loaded = false;
    vector<InputKey> m_keys;
};

// Source
struct InputSource
{
    obs_source_t *m_source = nullptr;
    uint32_t cx = 0;
    uint32_t cy = 0;
    string m_image_file;
    string m_layout_file;
    gs_image_file_t *m_image = nullptr;
    OverlayLayout m_layout;

    inline InputSource(obs_source_t *source_, obs_data_t *settings) :
        m_source(source_)
    {
        obs_source_update(m_source, settings);
    }

    inline ~InputSource()
    {
        UnloadOverlayTexure();
        UnloadOverlayLayout();
    }

    void LoadOverlayTexture(void);
    void LoadOverlayLayout(void);
    void UnloadOverlayLayout(void);
    void UnloadOverlayTexure(void);

    void DrawKey(gs_effect_t *effect, InputKey* key, uint16_t x, uint16_t y);
    void CheckKeys(void);

    inline bool io_begins(const std::string line, const std::string prefix)
    {
        if (prefix.size() > line.size()) return false;
        return equal(prefix.begin(), prefix.end(), line.begin());
    }

    inline void io_cut(std::string& line)
    {
        line = line.substr(line.rfind('=') + 1, line.size());
    }

    // Read key order from a string e.g. "A,B,C,0x10"
    unsigned char read_chain(std::string& l)
    {
        std::string temp = l.substr(0, l.find(','));
        l = l.substr(l.find(',') + 1, l.size());

        if (temp.length() < 2) {
            return temp[0];
        }
        else if (temp.find("0x") != std::string::npos)
        {
            //std::string t = std::string(temp.c_str());
            return std::stoul(temp, nullptr, 16);
        }
        return 'A';
    }

    uint16_t read_chain_int(std::string& l)
    {
        std::string temp = l.substr(0, l.find(','));
        l = l.substr(l.find(',') + 1, l.size());
        return std::atoi(temp.c_str());
    }

    inline void Update(obs_data_t *settings);
    inline void Tick(float seconds);
    inline void Render(gs_effect_t *effect);
};

void InputSource::DrawKey(gs_effect_t *effect, InputKey* key, uint16_t x, uint16_t y)
{
    gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"), m_image->texture);

    gs_matrix_push();
    gs_matrix_translate3f((float)x, (float)y, 1.f);
    if (key->m_pressed)
    {
        gs_draw_sprite_subregion(m_image->texture, 0, key->texture_u, key->texture_v + key->h + 3, key->w + 1, key->h + 1);
    }
    else
    {
        gs_draw_sprite_subregion(m_image->texture, 0, key->texture_u, key->texture_v, key->w + 1, key->h + 1);
    }

    gs_matrix_pop();
}

void InputSource::UnloadOverlayTexure()
{
    obs_enter_graphics();
    gs_image_file_free(m_image);
    obs_leave_graphics();
}

inline void InputSource::Update(obs_data_t *settings)
{
    m_image_file = obs_data_get_string(settings, S_OVERLAY_FILE);
    m_layout_file = obs_data_get_string(settings, S_LAYOUT_FILE);

    LoadOverlayTexture();
    LoadOverlayLayout();

    if (!m_layout.m_is_loaded && m_image && m_image->texture)
    {
        cx = m_image->cx;
        cy = m_image->cy;
    }
}

inline void InputSource::Tick(float seconds)
{
    CheckKeys();
}

inline void InputSource::Render(gs_effect_t *effect)
{
    if (!m_image || !m_image->texture)
        return;

    if (m_layout_file.empty() || !m_layout.m_is_loaded) {
        gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"), m_image->texture);
        gs_draw_sprite(m_image->texture, 0, cx, cy);
    }
    else {
        int x = 0, y = 0;
        InputKey k;

        if (m_layout.m_is_keyboard)
        {
            for (int i = 0; i < m_layout.m_key_count; i++)
            {
                k = m_layout.m_keys[i];
                x = m_layout.m_btn_w * k.column + (m_layout.m_key_space_h * k.column) + k.x_offset;
                y = m_layout.m_btn_h * k.row + (m_layout.m_key_space_v * k.row);
                DrawKey(effect, &k, x, y);
            }
        }
        else
        {
            for (int i = 0; i < m_layout.m_key_count; i++)
            {
                k = m_layout.m_keys[i];
                x = k.column;
                y = k.row;
                DrawKey(effect, &k, x, y);
            }
        }

    }
}

void InputSource::LoadOverlayTexture()
{
    UnloadOverlayTexure();

    if (!m_image_file.empty())
    {
        if (m_image == nullptr)
        {
            m_image = new gs_image_file_t();
            warning("Error: Image struct was null!");
        }

        warning("Loading texture %s as overlay base image!", m_image_file.c_str());
        gs_image_file_init(m_image, m_image_file.c_str());

        obs_enter_graphics();
        gs_image_file_init_texture(m_image);
        obs_leave_graphics();

        if (!m_image->loaded)
        {
            warning("Error: failed to load texture %s", m_image_file.c_str());
        }
    }
}

void InputSource::LoadOverlayLayout()
{
    m_layout.m_is_loaded = false;

    if (m_layout_file.empty())
        return;

    UnloadOverlayLayout();
    ifstream layout_file(m_layout_file.c_str());

    if (!layout_file.is_open())
        return;

    string line;
    string key_order, key_width, key_height, key_col, key_row;

    InputKey m[6]; // temporary storage for mouse keys
    short insert_mode = -1;
    short insert_index = -1;

    while (getline(layout_file, line)) {
        if (line[0] == '#')
            continue;

        if (io_begins(line, "layout_type")) {
            io_cut(line);
            m_layout.m_is_keyboard = line.find("1") != string::npos;
            continue;
        }
        else if (io_begins(line, "key_count")) {
            io_cut(line);
            m_layout.m_key_count = stoi(line);
            continue;
        }

        if (m_layout.m_is_keyboard)
        {
            if (io_begins(line, "key_rows")) {
                io_cut(line);
                m_layout.m_rows = stoi(line);
                continue;
            }
            else if (io_begins(line, "key_cols")) {
                io_cut(line);
                m_layout.m_cols = stoi(line);
                continue;
            }
            else if (io_begins(line, "key_abs_w")) {
                io_cut(line);
                m_layout.m_btn_w = stoi(line);
                continue;
            }
            else if (io_begins(line, "key_abs_h")) {
                io_cut(line);
                m_layout.m_btn_h = stoi(line);
                continue;
            }
            else if (io_begins(line, "key_space_v")) {
                io_cut(line);
                m_layout.m_key_space_v = stoi(line);
                continue;
            }
            else if (io_begins(line, "key_space_h")) {
                io_cut(line);
                m_layout.m_key_space_h = stoi(line);
                continue;
            }
            else if (io_begins(line, "key_order")) {
                io_cut(line);
                key_order = line;
                continue;
            }
            else if (io_begins(line, "key_width")) {
                io_cut(line);
                key_width = line;
                continue;
            }
            else if (io_begins(line, "key_height")) {
                io_cut(line);
                key_height = line;
                continue;
            }
            else if (io_begins(line, "key_col")) {
                io_cut(line);
                key_col = line;
                continue;
            }
            else if (io_begins(line, "key_row")) {
                io_cut(line);
                key_row = line;
                continue;
            }
            else if (io_begins(line, "texture_w")) {
                io_cut(line);
                m_layout.texture_w = stoi(line);
                continue;
            }
            else if (io_begins(line, "texture_v_space")) {
                io_cut(line);
                m_layout.texture_v_space = stoi(line);
                continue;
            }
        }
        else {
            if (io_begins(line, "mouse_lmb_u_v")) {
                m[0].m_key_char = KEY_LMB;
                insert_index = 0;
                insert_mode = 0;
            }
            else if (io_begins(line, "mouse_layout_w_h"))
            {
                io_cut(line);
                m_layout.m_w = read_chain_int(line);
                m_layout.m_h = read_chain_int(line);
            }
            else if (io_begins(line, "mouse_lmb_w_h")) {
                insert_index = 0;
                insert_mode = 1;
            }
            else if (io_begins(line, "mouse_lmb_x_y")) {
                insert_index = 0;
                insert_mode = 2;
            }
            else if (io_begins(line, "mouse_rmb_u_v")) {
                m[1].m_key_char = KEY_RMB;
                insert_index = 1;
                insert_mode = 0;
            }
            else if (io_begins(line, "mouse_rmb_w_h")) {
                insert_index = 1;
                insert_mode = 1;
            }
            else if (io_begins(line, "mouse_rmb_x_y")) {
                insert_index = 1;
                insert_mode = 2;
            }
            else if (io_begins(line, "mouse_mmb_u_v")) {
                m[2].m_key_char = KEY_MMB;
                insert_index = 2;
                insert_mode = 0;
            }
            else if (io_begins(line, "mouse_mmb_w_h")) {
                insert_index = 2;
                insert_mode = 1;
            }
            else if (io_begins(line, "mouse_mmb_x_y")) {
                insert_index = 2;
                insert_mode = 2;
            }
            else if (io_begins(line, "mouse_smb1_u_v")) {
                m[3].m_key_char = KEY_SMB1;
                insert_index = 3;
                insert_mode = 0;
            }
            else if (io_begins(line, "mouse_smb1_w_h")) {
                insert_index = 3;
                insert_mode = 1;
            }
            else if (io_begins(line, "mouse_smb1_x_y")) {
                insert_index = 3;
                insert_mode = 2;
            }
            else if (io_begins(line, "mouse_smb2_u_v")) {
                m[4].m_key_char = KEY_SMB2;
                insert_index = 4;
                insert_mode = 0;
            }
            else if (io_begins(line, "mouse_smb2_w_h")) {
                insert_index = 4;
                insert_mode = 1;
            }
            else if (io_begins(line, "mouse_smb2_x_y")) {
                insert_index = 4;
                insert_mode = 2;
            }
            else if (io_begins(line, "mouse_body_u_v")) {
                m[5].m_key_char = KEY_NONE;
                insert_index = 5;
                insert_mode = 0;
            }
            else if (io_begins(line, "mouse_body_w_h")) {
                insert_index = 5;
                insert_mode = 1;
            }
            else if (io_begins(line, "mouse_body_x_y")) {
                insert_index = 5;
                insert_mode = 2;
            }

            if (insert_mode == 0)
            {
                io_cut(line);
                m[insert_index].m_pressed = false;
                m[insert_index].texture_u = read_chain_int(line);
                m[insert_index].texture_v = read_chain_int(line);
            }
            else if (insert_mode == 1)
            {
                io_cut(line);
                m[insert_index].w = read_chain_int(line);
                m[insert_index].h = read_chain_int(line);
            }
            else if (insert_mode == 2)
            {
                io_cut(line);
                m[insert_index].column = read_chain_int(line);
                m[insert_index].row = read_chain_int(line);
            }
        }
    }

    if (m_layout.m_is_keyboard)
    {
        uint16_t u_cord = 1, v_cord = 1;
        uint16_t tempw, temph;
        int index = 0;
        for (int i = 0; i < m_layout.m_key_count; i++)
        {
            if (index >= m_layout.texture_w)
            {
                index = 0;
                u_cord = 1;
                v_cord += m_layout.texture_v_space + 6;
            }

            InputKey k;
            k.texture_u = u_cord - 1;
            k.texture_v = v_cord - 1;
            tempw = read_chain_int(key_width);
            temph = read_chain_int(key_height);
            k.w = tempw * m_layout.m_btn_w;
            k.h = temph * m_layout.m_btn_h;
            k.m_key_char = read_chain(key_order);
            k.m_pressed = false;
            k.row = read_chain_int(key_row);
            k.column = read_chain_int(key_col);

            if (tempw > 1)
            {
                k.x_offset = (m_layout.m_btn_w * tempw + m_layout.m_key_space_h * (tempw - 1)) / 2 - k.w / 2;
                index += tempw;
            }
            else
            {
                index++;
                k.x_offset = 0;
            }

            m_layout.m_keys.emplace_back(k);
            u_cord += k.w + 3;

        }

        m_layout.m_h = m_layout.m_rows * m_layout.m_btn_h + m_layout.m_key_space_v * m_layout.m_rows;
        m_layout.m_w = m_layout.m_cols * m_layout.m_btn_w + m_layout.m_key_space_h * (m_layout.m_cols - 1);
    }
    else
    {
        for (int i = 0; i < 6; i++)
            m_layout.m_keys.emplace_back(m[i]);
    }

    m_layout.m_key_count = min(m_layout.m_key_count, m_layout.m_keys.size());
    m_layout.m_is_loaded = true;

    cx = m_layout.m_w;
    cy = m_layout.m_h;
}

void InputSource::UnloadOverlayLayout()
{
    if (!m_layout.m_keys.empty())
        m_layout.m_keys.clear();
}

void InputSource::CheckKeys()
{
    if (m_layout.m_is_loaded) {
        for (int i = 0; i < m_layout.m_key_count; i++)
        {
            m_layout.m_keys[i].m_pressed = GetAsyncKeyState(m_layout.m_keys[i].m_key_char) & SHIFTED;
        }
    }
}

static obs_properties_t *get_properties(void *data)
{
    InputSource *s = reinterpret_cast<InputSource*>(data);

    obs_properties_t *props = obs_properties_create();

    string img_path;
    string layout_path;
    string filter_img;
    string filter_text;

    filter_text += T_FILTER_TEXT_FILES;
    filter_text += " (*.ini);;";
    filter_text += T_FILTER_ALL_FILES;
    filter_text += " (*.*)";

    filter_img += T_FILTER_IMAGE_FILES;
    filter_img += " (*.jpg *.png *.bmp);;";
    filter_img += T_FILTER_ALL_FILES;
    filter_img += " (*.*)";

    if (s && !s->m_image_file.empty()) {
        const char *slash;

        img_path = s->m_image_file;
        replace(img_path.begin(), img_path.end(), '\\', '/');
        slash = strrchr(img_path.c_str(), '/');
        if (slash)
            img_path.resize(slash - img_path.c_str() + 1);
    }

    if (s && !s->m_layout_file.empty()) {
        const char *slash;

        layout_path = s->m_layout_file;
        replace(layout_path.begin(), layout_path.end(), '\\', '/');
        slash = strrchr(layout_path.c_str(), '/');
        if (slash)
            layout_path.resize(slash - layout_path.c_str() + 1);
    }

    obs_properties_add_path(props, S_OVERLAY_FILE, T_OVERLAY_FILE, OBS_PATH_FILE,
        filter_img.c_str(), img_path.c_str());

    obs_properties_add_path(props, S_LAYOUT_FILE, T_LAYOUT_FILE, OBS_PATH_FILE,
        filter_text.c_str(), layout_path.c_str());
    return props;
}


// ---------------- Input History ----------------
// Lang & Value names
#define S_OVERLAY_FONT                  "font"
#define S_OVERLAY_FONT_COLOR            "color"
#define S_OVERLAY_DIRECTION             "direction_up"
#define S_OVERLAY_DIRECTION_LEFT        "left"
#define S_OVERLAY_DIRECTION_RIGHT       "right"
#define S_OVERLAY_HISTORY_SIZE          "history_size"
#define S_OVERLAY_FIX_CUTTING           "fix_cutting"
#define S_OVERLAY_INCLUDE_MOUSE	        "include_mouse"
#define S_OVERLAY_INTERVAL              "interval"
#define S_OVERLAY_CLEAR_HISTORY         "clear_history"

#define S_OVERLAY_OUTLINE               "outline"
#define S_OVERLAY_OUTLINE_SIZE          "outline_size"
#define S_OVERLAY_OUTLINE_COLOR         "outline_color"
#define S_OVERLAY_OUTLINE_OPACITY       "outline_opacity"

#define T_(v)                           obs_module_text(v)
#define T_OVERLAY_FONT                  T_("OverlayFont")
#define T_OVERLAY_FONT_COLOR            T_("OverlayFontColor")
#define T_OVERLAY_DIRECTION_LABEL       T_("OverlayDirection.Label")
#define T_OVERLAY_HISTORY_SIZE          T_("OverlayHistory.Size")
#define T_OVERLAY_FIX_CUTTING           T_("Overlay.FixCutting")
#define T_OVERLAY_INCLUDE_MOUSE         T_("Overlay.IncludeMouse")
#define T_OVERLAY_CLEAR_HISTORY         T_("Overlay.ClearHistory")

#define T_OVERLAY_OUTLINE               T_("Overlay.Outline")
#define T_OVERLAY_OUTLINE_SIZE          T_("Overlay.Outline.Size")
#define T_OVERLAY_OUTLINE_COLOR         T_("Overlay.Outline.Color")
#define T_OVERLAY_OUTLINE_OPACITY       T_("Overlay.Outline.Opacity")
#define T_OVERLAY_INTERVAL              T_("Overlay.Update.Interval")

#define ALPHABET_START  0x41
#define ALPHABET_END    0x5A
#define NUMBER_START    0x30
#define NUMBER_END      0x39
#define NUMPAD_START    0x60
#define NUMPAD_END      0x6F
#define FUNCTION_START  0x70
#define FUNCTION_END    0x87
#define UTIL_START      0x20
#define UTIL_END        0x2F
#define SPECIAL_SIZE 8

static int SPECIAL_KEYS[] = { VK_RETURN, VK_BACK, VK_TAB, VK_CAPITAL, VK_ESCAPE, VK_SCROLL, VK_PAUSE,
                              VK_NUMLOCK};

struct KeyBundle {
    bool m_empty = true;
    int m_keys[4] = { 0, 0, 0, 0 };
    bool m_mod_shift = false, m_mod_alt = false, m_mod_ctrl = false, m_mod_win = false;
    bool m_rmb = false, m_lmb = false, m_mmb = false, m_pmb = false, m_nmb = false;

    KeyBundle merge(KeyBundle other);

    const char* to_string(bool fix, bool include_mouse);
    bool compare(KeyBundle* other);
private:
    char* add_key(char* text, char* key, bool* flag);

};

struct InputHistorySource
{
    obs_source_t *m_source = nullptr;
    obs_source_t *m_text_source = nullptr;

    uint8_t m_history_size = 1;

    uint32_t cx = 0;
    uint32_t cy = 0;
    uint32_t m_update_interval = 1, m_counter = 0;

    bool m_fix_cutting = false;
    bool m_include_mouse = false;
    bool m_update_keys = false;
    bool m_collect_keys = false;
    bool m_dir_up = false;
    
    KeyBundle m_current_keys;
    KeyBundle m_prev_keys;
    KeyBundle m_history[5];

    inline InputHistorySource(obs_source_t *source_, obs_data_t *settings) :
        m_source(source_)
    {
        m_text_source = obs_source_create("text_gdiplus\0", "history-text-source", settings, NULL);
        obs_source_add_active_child(m_source, m_text_source);
        obs_source_update(m_source, settings);
        obs_source_update(m_text_source, settings);
    }

    inline ~InputHistorySource()
    {
        UnloadTextSource();
    }

    void UnloadTextSource(void);

    bool any_key_down(void);
    bool check_range(int start, int end);
    bool is_pressed(int k);
    void add_to_history(KeyBundle b);
    void clear_history(void);
    KeyBundle check_keys(void);

    inline void Update(obs_data_t *settings);
    inline void Tick(float seconds);
    inline void Render(gs_effect_t *effect);
};

void InputHistorySource::UnloadTextSource(void)
{
    if (m_text_source)
    {
        obs_source_remove(m_text_source);
        obs_source_release(m_text_source);
    }
    m_text_source = nullptr;
}

bool InputHistorySource::any_key_down(void)
{
    bool mods = is_pressed(VK_CONTROL) || is_pressed(VK_MENU) || is_pressed(VK_SHIFT) || is_pressed(VK_LWIN)
        || is_pressed(VK_RWIN) || is_pressed(VK_LBUTTON) || is_pressed(VK_RBUTTON) || is_pressed(VK_MBUTTON);

    bool special = false;
    for (int i = 0; i < SPECIAL_SIZE; i++)
    {
        if (is_pressed(SPECIAL_KEYS[i]))
        {
            special = true;
            break;
        }
    }

    return special || mods || check_range(ALPHABET_START, ALPHABET_END) || check_range(NUMBER_START, NUMBER_END)
        || check_range(NUMPAD_START, NUMPAD_END) || check_range(FUNCTION_START, FUNCTION_END)
        || check_range(UTIL_START, UTIL_END);
}

bool InputHistorySource::check_range(int start, int end)
{
    for (int i = start; i <= end; i++)
        if (GetAsyncKeyState(i) & SHIFTED)
            return true;
    return false;
}

bool InputHistorySource::is_pressed(int k)
{
    return GetAsyncKeyState(k) & SHIFTED;
}

void InputHistorySource::add_to_history(KeyBundle b)
{
    m_history[4] = m_history[3];
    m_history[3] = m_history[2];
    m_history[2] = m_history[1];
    m_history[1] = m_history[0];
    m_history[0] = b;
}

void InputHistorySource::clear_history(void)
{
    for (int i = 0; i < 4; i++)
    {
        m_history[i] = {};
    }
    obs_data_t* data = obs_source_get_settings(m_text_source);
    obs_data_set_string(data, "text", "");
    obs_source_update(m_text_source, data);
}

KeyBundle InputHistorySource::check_keys(void)
{
    KeyBundle temp = KeyBundle();
    temp.m_empty = false;
    temp.m_mod_ctrl = is_pressed(VK_CONTROL);
    temp.m_mod_shift = is_pressed(VK_SHIFT);
    temp.m_mod_alt = is_pressed(VK_MENU);
    temp.m_mod_win = is_pressed(VK_LWIN) || is_pressed(VK_RWIN);
    temp.m_lmb = is_pressed(VK_LBUTTON);
    temp.m_rmb = is_pressed(VK_RBUTTON);
    temp.m_mmb = is_pressed(VK_MBUTTON);
    temp.m_pmb = is_pressed(VK_XBUTTON1);
    temp.m_nmb = is_pressed(VK_XBUTTON2);

    int keys = 0;

    for (int i = ALPHABET_START; i <= ALPHABET_END; i++)
    {
        if (is_pressed(i))
        {
            temp.m_keys[keys] = i;
            keys++;
        }

        if (keys == 4)
        {
            return temp;
        }
    }

    for (int i = NUMBER_START; i <= NUMBER_END; i++)
    {
        if (is_pressed(i))
        {
            temp.m_keys[keys] = i;
            keys++;
        }

        if (keys == 4)
        {
            return temp;
        }
    }

    for (int i = NUMPAD_START; i <= NUMPAD_END; i++)
    {
        if (is_pressed(i))
        {
            temp.m_keys[keys] = i;
            keys++;
        }

        if (keys == 4)
        {
            return temp;
        }
    }

    for (int i = FUNCTION_START; i <= FUNCTION_END; i++)
    {
        if (is_pressed(i))
        {
            temp.m_keys[keys] = i;
            keys++;
        }

        if (keys == 4)
        {
            return temp;
        }
    }

    for (int i = 0; i < SPECIAL_SIZE; i++)
    {
        if (is_pressed(SPECIAL_KEYS[i]))
        {
            temp.m_keys[keys] = SPECIAL_KEYS[i];
            keys++;
        }

        if (keys == 4)
        {
            return temp;
        }
    }

    for (int i = UTIL_START; i <= UTIL_END; i++)
    {
        if (is_pressed(i))
        {
            temp.m_keys[keys] = i;
            keys++;
        }

        if (keys == 4)
        {
            return temp;
        }
    }
    return temp;
}

inline void InputHistorySource::Update(obs_data_t * settings)
{
    m_history_size = obs_data_get_int(settings, S_OVERLAY_HISTORY_SIZE);
    obs_source_update(m_text_source, settings);

    m_fix_cutting = obs_data_get_bool(settings, S_OVERLAY_FIX_CUTTING);
    m_include_mouse = obs_data_get_bool(settings, S_OVERLAY_INCLUDE_MOUSE);
    m_dir_up = obs_data_get_bool(settings, S_OVERLAY_DIRECTION);
    m_update_interval = obs_data_get_int(settings, S_OVERLAY_INTERVAL);
}

inline void InputHistorySource::Tick(float seconds)
{
    if (!obs_source_showing(m_source))
        return;

    cx = max(obs_source_get_width(m_text_source), 50);
    cy = max(obs_source_get_height(m_text_source), 50);
    
    if (m_update_keys)
    {
        obs_data_t* data = obs_source_get_settings(m_text_source);
        m_update_keys = false;
        if (!m_current_keys.m_empty && !m_current_keys.compare(&m_prev_keys))
            add_to_history(m_current_keys);
        m_prev_keys = m_current_keys;
        char* text = "";
        int i = m_dir_up ? m_history_size - 1: 0;
        for (;;)
        {
            if (m_history[i].m_empty || !m_dir_up && i >= m_history_size || m_dir_up && i < 0)
                break;

            if (!m_dir_up && i > 0 || m_dir_up && i < m_history_size - 1)
                text = append(text, "\n");
            text = append(text, (char*)m_history[i].to_string(m_fix_cutting, m_include_mouse));
            i += m_dir_up ? -1 : 1;
        }

        obs_data_set_string(data, "text", text);
        obs_source_update(m_text_source, data);
        m_current_keys = {};
    }
    else
    {
        if (any_key_down())
        {
            m_collect_keys = true;
        }
            
        if (m_collect_keys)
        {
            KeyBundle temp = check_keys();
            m_current_keys = m_current_keys.merge(temp);
            m_counter++;
            if (m_counter >= m_update_interval)
            {
                m_collect_keys = false;
                m_update_keys = true;
                m_counter = 0;
            }
        }
    }
}

inline void InputHistorySource::Render(gs_effect_t * effect)
{
    obs_source_video_render(m_text_source);
}



static bool clear_history(obs_properties_t *props, obs_property_t *property, void *data)
{
    InputHistorySource *source = reinterpret_cast<InputHistorySource*>(data);
    source->clear_history();
    return true;
}

// Properties
static obs_properties_t *get_properties_for_history(void *data)
{
    InputHistorySource *s = reinterpret_cast<InputHistorySource*>(data);

    obs_properties_t *props = obs_properties_create();

    // font settings
    obs_properties_add_font(props, S_OVERLAY_FONT, T_OVERLAY_FONT);
    obs_properties_add_color(props, S_OVERLAY_FONT_COLOR, T_OVERLAY_FONT_COLOR);
    
    obs_properties_add_bool(props, S_OVERLAY_OUTLINE, T_OVERLAY_OUTLINE);
    
    obs_properties_add_int(props, S_OVERLAY_OUTLINE_SIZE, T_OVERLAY_OUTLINE_SIZE, 1, 20, 1);
    obs_properties_add_color(props, S_OVERLAY_OUTLINE_COLOR, T_OVERLAY_OUTLINE_COLOR);
    obs_properties_add_int_slider(props, S_OVERLAY_OUTLINE_OPACITY,
        T_OVERLAY_OUTLINE_OPACITY, 0, 100, 1);

    obs_properties_add_bool(props, S_OVERLAY_DIRECTION, T_OVERLAY_DIRECTION_LABEL);
    obs_properties_add_int(props, S_OVERLAY_HISTORY_SIZE, T_OVERLAY_HISTORY_SIZE, 1, 5, 1);
    obs_properties_add_bool(props, S_OVERLAY_FIX_CUTTING, T_OVERLAY_FIX_CUTTING);
    obs_properties_add_bool(props, S_OVERLAY_INCLUDE_MOUSE, T_OVERLAY_INCLUDE_MOUSE);
    obs_properties_add_int(props, S_OVERLAY_INTERVAL, T_OVERLAY_INTERVAL, 1, 1000, 1);

    obs_properties_add_button(props, S_OVERLAY_CLEAR_HISTORY, T_OVERLAY_CLEAR_HISTORY, clear_history);
    return props;
}

// --------------------- End ---------------------

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("input-overlay", "en-US")

bool obs_module_load(void)
{
    // Input Overlay
    obs_source_info si = {};
    si.id = "input-overlay";
    si.type = OBS_SOURCE_TYPE_INPUT;
    si.output_flags = OBS_SOURCE_VIDEO;
    si.get_properties = get_properties;

    si.get_name = [](void*) { return obs_module_text("InputOverlay"); };
    si.create = [](obs_data_t *settings, obs_source_t *source) { return (void*) new InputSource(source, settings);    };
    si.destroy = [](void *data) { delete reinterpret_cast<InputSource*>(data);    };
    si.get_width = [](void *data) { return reinterpret_cast<InputSource*>(data)->cx;    };
    si.get_height = [](void *data) { return reinterpret_cast<InputSource*>(data)->cy;    };

    si.get_defaults = [](obs_data_t *settings)
    {
        obs_data_t *input_obj = obs_data_create();
        obs_data_release(input_obj);
    };

    si.update = [](void *data, obs_data_t *settings) { reinterpret_cast<InputSource*>(data)->Update(settings); };
    si.video_tick = [](void *data, float seconds) { reinterpret_cast<InputSource*>(data)->Tick(seconds); };
    si.video_render = [](void *data, gs_effect_t *effect) { reinterpret_cast<InputSource*>(data)->Render(effect); };
    obs_register_source(&si);

    // Input History
    si = {};
    si.id = "input-history";
    si.type = OBS_SOURCE_TYPE_INPUT;
    si.output_flags = OBS_SOURCE_VIDEO;
    si.get_properties = get_properties_for_history;

    si.get_name = [](void*) { return obs_module_text("InputHistory"); };
    si.create = [](obs_data_t *settings, obs_source_t *source) { return (void*) new InputHistorySource(source, settings); };
    si.destroy = [](void *data) { delete reinterpret_cast<InputHistorySource*>(data); };
    si.get_width = [](void *data) { return reinterpret_cast<InputHistorySource*>(data)->cx; };
    si.get_height = [](void *data) { return reinterpret_cast<InputHistorySource*>(data)->cy; };

    si.get_defaults = [](obs_data_t *settings)
    {
        obs_data_t *input_obj = obs_data_create();
        obs_data_release(input_obj);
    };

    si.update = [](void *data, obs_data_t *settings) { reinterpret_cast<InputHistorySource*>(data)->Update(settings); };
    si.video_tick = [](void *data, float seconds) { reinterpret_cast<InputHistorySource*>(data)->Tick(seconds); };
    si.video_render = [](void *data, gs_effect_t *effect) { reinterpret_cast<InputHistorySource*>(data)->Render(effect); };
    obs_register_source(&si);

    return true;
}

KeyBundle KeyBundle::merge(KeyBundle other)
{
    KeyBundle c;
    if (other.m_empty)
        return c;
    c.m_empty       = false;
    c.m_mmb         = other.m_mmb || m_mmb;
    c.m_lmb         = other.m_lmb || m_lmb;
    c.m_rmb         = other.m_rmb || m_rmb;
    c.m_mod_alt     = other.m_mod_alt || m_mod_alt;
    c.m_mod_ctrl    = other.m_mod_ctrl || m_mod_ctrl;
    c.m_mod_shift   = other.m_mod_shift || m_mod_shift;
    c.m_mod_win     = other.m_mod_win || m_mod_win;

    for (int i = 0; i < 4; i++)
    {
        if (other.m_keys[i] > 0)
            c.m_keys[i] = other.m_keys[i];
        else
            c.m_keys[i] = m_keys[i];
    }
    return c;
}

const char * KeyBundle::to_string(bool fix, bool include_mouse)
{
    if (m_empty)
        return "";

    char* text = "";
    bool flag = false;

    if (m_mod_shift)
        text = add_key(text, "SHIFT", &flag);

    if (m_mod_ctrl)
        text = add_key(text, "CTRL", &flag);

    if (m_mod_alt)
        text = add_key(text, "ALT", &flag);

    if (m_mod_win)
        text = add_key(text, "WIN", &flag);

    if (include_mouse)
    {
        if (m_lmb)
            text = add_key(text, "LEFT CLICK", &flag);
        if (m_rmb)
            text = add_key(text, "RIGHT CLICK", &flag);
        if (m_mmb)
            text = add_key(text, "MIDDLE CLICK", &flag);
    }

    for (int i = 0; i < 4; i++)
    {
        if (m_keys[i] > 0)
            text = add_key(text, (char*)key_to_text(m_keys[i]), &flag);
    }

    if (fix)
        text = append(text, " ");
    return text;
}

char * KeyBundle::add_key(char * text, char * key, bool * flag)
{
    if (*flag)
        text = append(text, " + ");
    *flag = true;
    text = append(text, key);
    return text;
}

bool KeyBundle::compare(KeyBundle * other)
{
    if (m_empty != other->m_empty)
        return false;

    if (m_lmb != other->m_lmb || m_rmb != other->m_rmb || m_mmb != other->m_mmb
        || m_pmb != other->m_pmb || m_nmb != other->m_nmb)
        return false;

    if (m_mod_alt != other->m_mod_alt || m_mod_ctrl != other->m_mod_ctrl
        || m_mod_win != other->m_mod_win || m_mod_shift != other->m_mod_shift)
        return false;

    for (int i = 0; i < 4; i++)
    {
        if (m_keys[i] != other->m_keys[i])
            return false;
    }
    return true;
}