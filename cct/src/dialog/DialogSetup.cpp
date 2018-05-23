/**
 * Created by universallp on 23.05.2018.
 * This file is part of input-overlay which is licenced
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "DialogSetup.hpp"

void DialogSetup::init()
{
	Dialog::init();
	m_screen_elements.emplace_back(new Label(1, 5, 10, "Choose a texture file", this));
}

void DialogSetup::action_performed(int8_t action_id)
{

}
