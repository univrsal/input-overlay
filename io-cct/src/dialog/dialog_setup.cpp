/**
 * Created by univrsal on 23.05.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#include "dialog_setup.hpp"
#include "../util/notifier.hpp"
#include "../util/constants.hpp"
#include "../tool.hpp"
#include "../util/localization.hpp"

#if _DEBUG
#define TEXTURE_PATH    "D:\\Projects\\prog\\cpp\\input-overlay-releases\\build\\v4.6-pre\\presets\\wasd-full\\wasd.png"
#define CONFIG_PATH     "C:\\Users\\user\\Desktop\\test.ini"
#else
#define TEXTURE_PATH    m_tool->get_texture_path()
#define CONFIG_PATH     m_tool->get_config_path()
#endif

void dialog_setup::init()
{
	dialog::init();
	int8_t id = 1;

	// info labels
	auto info = std::string(LABEL_BUILD);
	info.append(std::to_string(BUILD_NUMBER));

	add(new label(id++, 8, 22, info.c_str(), FONT_WSTRING_LARGE, this,
	              ELEMENT_UNLOCALIZED | ELEMENT_ABSOLUTE_POSITION));
	add(new label(id++, 8, 50, LANG_LABEL_INFO, this, ELEMENT_ABSOLUTE_POSITION));

	add(new label(id++, 8, 35, LANG_LABEL_TEXTURE_PATH, this));
	add(m_texture_path = new textbox(id++, 8, 55, m_dimensions.w - 16, 20, TEXTURE_PATH, this));

	add(new label(id++, 8, 85, LANG_LABEL_CONFIG_PATH, this));
	add(m_config_path = new textbox(id++, 8, 105, m_dimensions.w - 16, 20, CONFIG_PATH, this));

	add(new label(id++, 8, 135, LANG_LABEL_DEFAULT_WIDTH, this));
	add(new label(id++, (m_dimensions.w / 2) + 4, 135, LANG_LABEL_DEFAULT_HEIGHT, this));

	add(new label(id++, 8, 185, LANG_LABEL_ELEMENT_H_SPACE, this));
	add(new label(id++, (m_dimensions.w / 2) + 4, 185, LANG_LABEL_ELEMENT_V_SPACE, this));

	add(m_def_w = new textbox(id++, 8, 155, (m_dimensions.w / 2) - 16, 20, "0", this));
	add(m_def_h = new textbox(id++, (m_dimensions.w / 2) + 4, 155, (m_dimensions.w / 2) - 12, 20, "0", this));

	add(m_h_space = new textbox(id++, 8, 205, (m_dimensions.w / 2) - 16, 20, "0", this));
	add(m_v_space = new textbox(id++, (m_dimensions.w / 2) + 4, 205, (m_dimensions.w / 2) - 12, 20, "0", this));

	m_def_w->set_flags(TEXTBOX_NUMERIC);
	m_def_h->set_flags(TEXTBOX_NUMERIC);

	m_h_space->set_flags(TEXTBOX_NUMERIC);
	m_v_space->set_flags(TEXTBOX_NUMERIC);

	m_config_path->set_flags(TEXTBOX_DROP_FILE);
	m_texture_path->set_flags(TEXTBOX_DROP_FILE);

	add(new button(ACTION_OK, 8, m_dimensions.h - 32, LANG_BUTTON_OK, this));
	add(new button(ACTION_CANCEL, 116, m_dimensions.h - 32, LANG_BUTTON_EXIT, this));

	add(m_lang_box = new combobox(id++, m_dimensions.w - 148, m_dimensions.h - 28, 140, 20, this, ELEMENT_UNLOCALIZED));

	const auto files = m_helper->get_localization()->get_languages();

	for (auto const &f : *files) {
		m_lang_box->add_item(f->language);
	}

	m_lang_box->select_item(m_helper->get_localization()->get_english_id());

	set_flags(DIALOG_CENTERED | DIALOG_TEXTINPUT);
}

void dialog_setup::draw_background()
{
	dialog::draw_background();
}

void dialog_setup::action_performed(const int8_t action_id)
{
	dialog::action_performed(action_id);

	auto valid_texture = false;
	auto empty_config = false;
	ccl_config *cfg = nullptr;

	switch (action_id) {
	case ACTION_OK:
		valid_texture = m_helper->util_check_texture_path(m_texture_path->get_text()->c_str());
		cfg = new ccl_config(*m_config_path->get_text(), "");
		m_load_cfg = cfg->can_write();
		empty_config = cfg->is_empty();

		if (!m_texture_path->get_text()->empty() && !m_config_path->get_text()->empty() && valid_texture &&
		    m_load_cfg) {
			m_tool->action_performed(TOOL_ACTION_SETUP_EXIT);
		} else {
			if (m_texture_path->get_text()->empty() || !valid_texture) {
				m_texture_path->set_alert(true);
				m_notifier->add_msg(MESSAGE_ERROR, m_helper->loc(LANG_ERROR_INVALID_TEXTURE_PATH));
			}

			if (m_config_path->get_text()->empty() || !m_load_cfg) {
				m_config_path->set_alert(true);
				m_notifier->add_msg(MESSAGE_ERROR, m_helper->loc(LANG_ERROR_INVALID_CONFIG_PATH));
			}
		}
		break;
	case ACTION_CANCEL:
		m_helper->exit_loop();
		break;
	case ACTION_FILE_DROPPED:
		cfg = new ccl_config(*m_config_path->get_text(), "");
		if (!cfg->is_empty()) {
			const auto def_w = cfg->get_node(CFG_DEFAULT_WIDTH, true);
			const auto def_h = cfg->get_node(CFG_DEFAULT_HEIGHT, true);
			const auto space_h = cfg->get_node(CFG_H_SPACE, true);
			const auto space_v = cfg->get_node(CFG_V_SPACE, true);

			if (def_w)
				m_def_w->set_text(def_w->get_value());
			if (def_h)
				m_def_h->set_text(def_h->get_value());
			if (space_h)
				m_h_space->set_text(space_h->get_value());
			if (space_v)
				m_v_space->set_text(space_v->get_value());
		}
		break;
	case ACTION_COMBO_ITEM_SELECTED:
		m_helper->get_localization()->load_lang_by_id(m_lang_box->get_selected());
		reload_lang();
		break;
	default: ;
	}

	if (cfg) {
		delete cfg;
		cfg = nullptr;
	}
}

const char *dialog_setup::get_config_path() const
{
	return m_config_path->get_text()->c_str();
}

const char *dialog_setup::get_texture_path() const
{
	return m_texture_path->get_text()->c_str();
}

SDL_Point dialog_setup::get_rulers() const
{
	return SDL_Point{std::stoi(m_h_space->get_text()->c_str()), std::stoi(m_v_space->get_text()->c_str())};
}

SDL_Point dialog_setup::get_default_dim() const
{
	return SDL_Point{std::stoi(m_def_w->get_text()->c_str()), std::stoi(m_def_h->get_text()->c_str())};
}

bool dialog_setup::should_load_cfg() const
{
	return m_load_cfg;
}
