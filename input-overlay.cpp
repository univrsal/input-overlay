#include <obs-module.h>
extern "C" {
#include <graphics\image-file.h>
}
#include <graphics\vec2.h>
#include <string>
#include <vector>
#include <algorithm>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <thread>

using namespace std;
#define SHIFTED 0x8000 

#define KEY_LMB		VK_LBUTTON
#define KEY_RMB		VK_RBUTTON
#define KEY_MMB		VK_MBUTTON
#define KEY_SMB1	VK_XBUTTON1
#define KEY_SMB2	VK_XBUTTON2
#define KEY_NONE	0x07

#define MIN_SIZE_CX 2
#define MIN_SIZE_CY 2
#define MAX_SIZE_CX 16384
#define MAX_SIZE_CY 16384

#define MAX_AREA (4096LL * 4096LL)

#define warning(format, ...) blog(LOG_WARNING, "[%s] " format, \
		obs_source_get_name(m_source), ##__VA_ARGS__)
// Lang
#define S_OVERLAY_FILE					"overlay_image"
#define S_LAYOUT_FILE					"layout_file"

#define T_(v)							obs_module_text(v)
#define T_OVERLAY_FILE					T_("OverlayFile")
#define T_LAYOUT_FILE					T_("LayoutFile")
#define T_FILTER_IMAGE_FILES            T_("Filter.ImageFiles")
#define T_FILTER_TEXT_FILES				T_("Filter.TextFiles")
#define T_FILTER_ALL_FILES              T_("Filter.AllFiles")

// Util structs
struct InputKey {
	char m_key_char;
	bool m_pressed;
	uint16_t texture_u, texture_v;
	uint16_t w, h;
	
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
struct InputSource {
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

	static void UpdateScroll();

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
	char read_chain(std::string& l)
	{
		std::string temp = l.substr(0, l.find(','));
		l = l.substr(l.find(',') + 1, l.size());

		if (temp.length() < 2) {
			return temp[0];
		}
		else if (temp.find("0x") != std::string::npos)
		{
			std::string t = std::string(temp.c_str());
			return std::stoul(t, nullptr, 16);
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
	gs_matrix_translate3f((float) x, (float) y, 1.f);
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
	} else {
		int x = 0, y = 0;
		InputKey k;

		if (m_layout.m_is_keyboard)
		{
			for (int i = 0; i < m_layout.m_key_count; i++)
			{
				k = m_layout.m_keys[i];
				x = m_layout.m_btn_w * k.column + (m_layout.m_key_space_h * k.column);
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
			else if (io_begins(line,"key_abs_h")) {
				io_cut(line);
				m_layout.m_btn_h = stoi(line);
				continue;
			}
			else if (io_begins(line, "key_space_v")) {
				io_cut(line);
				m_layout.m_key_space_v = stoi(line);
				continue;
			}
			else if (io_begins(line,"key_space_h")) {
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
			} else if (io_begins(line, "mouse_lmb_w_h")) {
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
				insert_index =3;
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
		int row_index = 0;
		for (int i = 0; i < m_layout.m_key_count; i++)
		{
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
			m_layout.m_keys.emplace_back(k);

			u_cord += k.w + 3;

			if (i == m_layout.texture_w - 1)
			{
				row_index++;
				u_cord = 1;
				v_cord += m_layout.texture_v_space + (6 * row_index);
			}
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
			m_layout.m_keys[i].m_pressed = GetKeyState(m_layout.m_keys[i].m_key_char) & SHIFTED;
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

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("input-overlay", "en-US")

std::thread monitor_thread;

bool obs_module_load(void)
{
	obs_source_info si = {};
	si.id = "input-overlay";
	si.type = OBS_SOURCE_TYPE_INPUT;
	si.output_flags = OBS_SOURCE_VIDEO;
	si.get_properties = get_properties;

	si.get_name = [] (void*) { return obs_module_text("InputOverlay"); };
	si.create = [] (obs_data_t *settings, obs_source_t *source) { return (void*) new InputSource(source, settings);  };
	si.destroy = [] (void *data) { delete reinterpret_cast<InputSource*>(data);  };
	si.get_width = [] (void *data) { return reinterpret_cast<InputSource*>(data)->cx;  };
	si.get_height = [] (void *data) { return reinterpret_cast<InputSource*>(data)->cy;  };
	
	si.get_defaults = [](obs_data_t *settings)
	{
		obs_data_t *input_obj = obs_data_create();
		obs_data_release(input_obj);
	};

	si.update = [] (void *data, obs_data_t *settings) { reinterpret_cast<InputSource*>(data)->Update(settings); };
	si.video_tick = [](void *data, float seconds) { reinterpret_cast<InputSource*>(data)->Tick(seconds); };
	si.video_render = [](void *data, gs_effect_t *effect) { reinterpret_cast<InputSource*>(data)->Render(effect); };

	obs_register_source(&si);
	return true;
}