/**
 * Created by univrsal on 23.05.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "DialogSetup.hpp"
#include "../util/Notifier.hpp"
#include "../util/Constants.hpp"
#include "../Tool.hpp"
#include "../util/Localization.hpp"

#if _DEBUG
#define TEXTURE_PATH "D:\\Projects\\prog\\cpp\\input-overlay-releases\\build\\v4.6-pre\\presets\\\wasd-full\\wasd.png"
#define CONFIG_PATH "C:\\Users\\user\\Desktop\\test.ini"
#else
#define TEXTURE_PATH ""
#define CONFIG_PATH ""
#endif

void DialogSetup::init()
{
	Dialog::init();
	int8_t id = 1;

	// info labels

	std::string info = std::string(LABEL_BUILD);
	info.append(std::to_string(BUILD_NUMBER));

	add(new Label(id++, 8, 28, info.c_str(), FONT_ROBOTO_LARGE, this, ELEMENT_UNLOCALIZED | ELEMENT_ABSOLUTE_POSITION));
	add(new Label(id++, 8, 48, LANG_LABEL_INFO, this, ELEMENT_ABSOLUTE_POSITION));

	add(new Label(id++, 8, 35, LANG_LABEL_TEXTURE_PATH, this));
	add(m_texture_path = new Textbox(id++, 8, 55, m_dimensions.w - 16, 20, TEXTURE_PATH, this));

	add(new Label(id++, 8, 85, LANG_LABEL_CONFIG_PATH, this));
	add(m_config_path = new Textbox(id++, 8, 105, m_dimensions.w - 16, 20, CONFIG_PATH, this));

	add(new Label(id++, 8, 135, LANG_LABEL_DEFAULT_WIDTH, this));
	add(new Label(id++, (m_dimensions.w / 2) + 4, 135, LANG_LABEL_DEFAULT_HEIGHT, this));

	add(new Label(id++, 8, 185, LANG_LABEL_ELEMENT_H_SPACE, this));
	add(new Label(id++, (m_dimensions.w / 2) + 4, 185, LANG_LABEL_ELEMENT_V_SPACE, this));

	add(m_def_w = new Textbox(id++, 8, 155, (m_dimensions.w / 2) - 16, 20, "0", this));
	add(m_def_h = new Textbox(id++, (m_dimensions.w / 2) + 4, 155, (m_dimensions.w / 2) - 12, 20, "0", this));

	add(m_h_space = new Textbox(id++, 8, 205, (m_dimensions.w / 2) - 16, 20, "0", this));
	add(m_v_space = new Textbox(id++, (m_dimensions.w / 2) + 4, 205, (m_dimensions.w / 2) - 12, 20, "0", this));

	m_def_w->set_flags(TEXTBOX_NUMERIC);
	m_def_h->set_flags(TEXTBOX_NUMERIC);

	m_h_space->set_flags(TEXTBOX_NUMERIC);
	m_v_space->set_flags(TEXTBOX_NUMERIC);

	m_config_path->set_flags(TEXTBOX_DROP_FILE);
	m_texture_path->set_flags(TEXTBOX_DROP_FILE);

	add(new Button(ACTION_OK, 8, m_dimensions.h - 32, LANG_BUTTON_OK, this));
	add(new Button(ACTION_CANCEL, 116, m_dimensions.h - 32, LANG_BUTTON_EXIT, this));

	add(m_lang_box = new Combobox(id++, m_dimensions.w - 148, m_dimensions.h - 28, 140, 20, this, ELEMENT_UNLOCALIZED));

	const std::vector<std::unique_ptr<LangFile>> * files = m_helper->localization()->get_languages();

	for (auto const& f : *files)
	{
		m_lang_box->add_item(f->language);
	}

	m_lang_box->select_item(m_helper->localization()->get_english_id());

	set_flags(DIALOG_CENTERED | DIALOG_TEXTINPUT);
}

void DialogSetup::draw_background(void)
{
	Dialog::draw_background();
}

void DialogSetup::action_performed(int8_t action_id)
{
	Dialog::action_performed(action_id);

	bool valid_texture = false;
	bool empty_config = false;
	ccl_config * cfg = nullptr;

	switch (action_id)
	{
	case ACTION_OK:
		valid_texture = m_helper->util_check_texture_path(m_texture_path->get_text()->c_str());
		cfg = new ccl_config(*m_config_path->get_text(), "");
		m_load_cfg = cfg->can_write();
		empty_config = cfg->is_empty();

		if (!m_texture_path->get_text()->empty() && !m_config_path->get_text()->empty() && valid_texture && m_load_cfg)
		{
			m_tool->action_performed(TOOL_ACTION_SETUP_EXIT);
		}
		else
		{
			if (m_texture_path->get_text()->empty() || !valid_texture)
			{
				m_texture_path->set_alert(true);
				m_notifier->add_msg(MESSAGE_ERROR, m_helper->loc(LANG_ERROR_INVALID_TEXTURE_PATH));
			}

			if (m_config_path->get_text()->empty() || !m_load_cfg)
			{
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
		if (!cfg->is_empty())
		{
			ccl_data * def_w = cfg->get_node(CFG_DEFAULT_WIDTH, true);
			ccl_data * def_h = cfg->get_node(CFG_DEFAULT_HEIGHT, true);
			ccl_data * space_h = cfg->get_node(CFG_H_SPACE, true);
			ccl_data * space_v = cfg->get_node(CFG_V_SPACE, true);

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
		m_helper->localization()->load_lang_by_id(m_lang_box->get_selected());
		reload_lang();
		break;
	}

	if (cfg)
	{
		delete cfg;
		cfg = nullptr;
	}
}

const char * DialogSetup::get_config_path(void)
{
	return m_config_path->get_text()->c_str();
}

const char * DialogSetup::get_texture_path(void)
{
	return m_texture_path->get_text()->c_str();
}

SDL_Point DialogSetup::get_rulers()
{
	return SDL_Point{ std::stoi(m_h_space->get_text()->c_str()),
		std::stoi(m_v_space->get_text()->c_str()) };
}

SDL_Point DialogSetup::get_default_dim()
{
	return SDL_Point { std::stoi(m_def_w->get_text()->c_str()),
		std::stoi(m_def_h->get_text()->c_str()) };
}

bool DialogSetup::should_load_cfg(void)
{
	return m_load_cfg;
}
