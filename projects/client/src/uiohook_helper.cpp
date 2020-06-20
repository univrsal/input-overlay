/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <uni@vrsal.cf>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#include "uiohook_helper.hpp"
#include "network.hpp"
#include "client_util.hpp"
#include <cstdarg>
#include <cstdio>
#include <util.hpp>

namespace uiohook {
std::mutex m_mutex;
data_holder data;
volatile bool hook_state = false;

data_holder::data_holder() : m_mouse_x(0), m_mouse_y(0), m_wheel_direction(wheel_none), m_new_mouse_data(false) {}

void data_holder::set_button(const uint16_t keycode, const bool pressed)
{
	m_mutex.lock();
	if (pressed)
		m_button_states[keycode] = pressed;
	else
		m_button_states.erase(keycode);
	m_mutex.unlock();
}

void data_holder::set_mouse_pos(const int16_t x, const int16_t y)
{
	m_mutex.lock();
	m_mouse_x = x;
	m_mouse_y = y;
	m_new_mouse_data = true;
	m_mutex.unlock();
}

void data_holder::set_wheel(int amount, wheel_dir dir)
{
	m_mutex.lock();
	if (dir != m_wheel_direction)
		m_wheel_amount = amount;
	else
		m_wheel_amount += amount;
	m_wheel_direction = dir;
	m_new_mouse_data = true;
	m_last_scroll = util::get_ticks();
	m_mutex.unlock();
}

void data_holder::reset_wheel()
{
	m_mutex.lock();
	m_new_mouse_data = true;
	m_wheel_direction = wheel_none;
	m_mutex.unlock();
}

void data_holder::set_wheel(bool pressed)
{
	m_mutex.lock();
	m_new_mouse_data = true;
	m_wheel_pressed = pressed;
	m_mutex.unlock();
}

bool data_holder::write_to_buffer(netlib_byte_buf *buffer)
{
	/* Message always sends both mouse and button data. */

	auto success = true;

	if (netlib_write_uint8(buffer, network::MSG_BUTTON_DATA)) {
		if (netlib_write_uint8(buffer, int(m_button_states.size()))) {
			for (const auto &data : m_button_states) {
				if (!netlib_write_uint16(buffer, data.first))
					success = false;
			}
		} else {
			success = false;
		}
	} else {
		success = false;
	}

	if (m_new_mouse_data) {
		success = netlib_write_uint8(buffer, network::MSG_MOUSE_DATA) && netlib_write_int16(buffer, m_mouse_x) &&
				  netlib_write_int16(buffer, m_mouse_y) && netlib_write_int8(buffer, m_wheel_direction) &&
				  netlib_write_int16(buffer, m_wheel_amount) && netlib_write_int8(buffer, m_wheel_pressed);

		/* TODO: write other mouse data */
		m_new_mouse_data = false;
	}

	return success;
}

uint32_t data_holder::get_last_scroll()
{
	return m_last_scroll;
}

bool logger_proc(unsigned level, const char *format, ...)
{
	auto status = false;

	va_list args;
	switch (level) {
	default:
#ifdef USE_DEBUG

	case LOG_LEVEL_DEBUG:
	case LOG_LEVEL_INFO:
		va_start(args, format);
		status = vfprintf(stdout, format, args) >= 0;
		va_end(args);

#endif
		break;
	case LOG_LEVEL_WARN:
	case LOG_LEVEL_ERROR:
		va_start(args, format);
		status = vfprintf(stderr, format, args) >= 0;
		va_end(args);
		break;
	}

	return status;
}

void dispatch_proc(uiohook_event *const event)
{
	switch (event->type) {
	case EVENT_HOOK_ENABLED:
		DEBUG_LOG("uiohook started\n");
		hook_state = true;
		break;
	case EVENT_HOOK_DISABLED:
		DEBUG_LOG("uiohook exited\n");
		break;
	case EVENT_MOUSE_CLICKED:
	case EVENT_MOUSE_PRESSED:
	case EVENT_MOUSE_RELEASED:
		if (util::cfg.monitor_mouse) {
			if (is_middle_mouse(event->data.mouse.button))
				data.set_wheel(event->type == EVENT_MOUSE_PRESSED);
			else
				data.set_button(util_mouse_fix(event->data.mouse.button) | VC_MOUSE_MASK,
								event->type == EVENT_MOUSE_PRESSED);
		}
		break;
	case EVENT_MOUSE_WHEEL:
		if (util::cfg.monitor_mouse) {
			data.set_wheel(event->data.wheel.amount, event->data.wheel.rotation >= WHEEL_DOWN ? wheel_down : wheel_up);
		} /* Fallthrough */
	case EVENT_MOUSE_MOVED:
	case EVENT_MOUSE_DRAGGED:
		if (util::cfg.monitor_mouse)
			data.set_mouse_pos(event->data.mouse.x, event->data.mouse.y);
		break;
	case EVENT_KEY_TYPED: /* TODO: how to handle this */
	case EVENT_KEY_PRESSED:
	case EVENT_KEY_RELEASED:
		if (util::cfg.monitor_keyboard)
			data.set_button(event->data.keyboard.keycode, event->type == EVENT_KEY_PRESSED);
		break;
	default:;
	}
}

bool init()
{
	hook_set_logger_proc(&logger_proc);
	hook_set_dispatch_proc(&dispatch_proc);

	const auto status = hook_run();

	switch (status) {
	case UIOHOOK_SUCCESS:
		hook_state = true;
		return true;
	case UIOHOOK_ERROR_OUT_OF_MEMORY:
		logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to allocate memory. (%#X)", status);
		return false;
	case UIOHOOK_ERROR_X_OPEN_DISPLAY:
		logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to open X11 display. (%#X)", status);
		return false;
	case UIOHOOK_ERROR_X_RECORD_NOT_FOUND:
		logger_proc(LOG_LEVEL_ERROR, "[uiohook] Unable to locate XRecord extension. (%#X)", status);
		return false;
	case UIOHOOK_ERROR_X_RECORD_ALLOC_RANGE:
		logger_proc(LOG_LEVEL_ERROR, "[uiohook] Unable to allocate XRecord range. (%#X)", status);
		return false;
	case UIOHOOK_ERROR_X_RECORD_CREATE_CONTEXT:
		logger_proc(LOG_LEVEL_ERROR, "[uiohook] Unable to allocate XRecord context. (%#X)", status);
		return false;
	case UIOHOOK_ERROR_X_RECORD_ENABLE_CONTEXT:
		logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to enable XRecord context. (%#X)", status);
		return false;
	case UIOHOOK_ERROR_SET_WINDOWS_HOOK_EX:
		logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to register low level windows hook. (%#X)", status);
		return false;
	case UIOHOOK_ERROR_AXAPI_DISABLED:
		logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to enable access for assistive devices. (%#X)", status);
		return false;
	case UIOHOOK_ERROR_CREATE_EVENT_PORT:
		logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to create apple event port. (%#X)", status);
		return false;
	case UIOHOOK_ERROR_CREATE_RUN_LOOP_SOURCE:
		logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to create apple run loop source. (%#X)", status);
		return false;
	case UIOHOOK_ERROR_GET_RUNLOOP:
		logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to acquire apple run loop. (%#X)", status);
		return false;
	case UIOHOOK_ERROR_CREATE_OBSERVER:
		logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to create apple run loop observer. (%#X)", status);
		return false;
	case UIOHOOK_FAILURE:
		logger_proc(LOG_LEVEL_ERROR, "[uiohook] An unknown hook error occurred. (%#X)", status);
		return false;
	default:;
		return true; /* Unknown error but we can still try */
	}
}

void close()
{
	if (!hook_state)
		return;
	hook_state = false;
	const auto status = hook_stop();

	DEBUG_LOG("Closing hook\n");
	switch (status) {
	case UIOHOOK_ERROR_OUT_OF_MEMORY:
		logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to allocate memory. (%#X)", status);
		break;
	case UIOHOOK_ERROR_X_RECORD_GET_CONTEXT:
		logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to get XRecord context. (%#X)", status);
		break;
	case UIOHOOK_FAILURE:
		logger_proc(LOG_LEVEL_ERROR, "[uiohook] An unknown hook error occurred. (%#X)", status);
		break;
	default:;
	}
}
}
