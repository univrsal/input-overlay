#ifndef HOOK_HELPER_HPP
#define HOOK_HELPER_HPP

#define MAX_KEYBOARD_KEYS 14
#define MAX_SIMULTANEOUS_KEYS (MAX_KEYBOARD_KEYS + PAD_BUTTON_COUNT)

#ifdef LINUX
#include <stdint.h>
#endif
#include <uiohook.h>
#include <stdarg.h>
#include "../util/util.hpp"

#ifdef WINDOWS
#include <windows.h>
#endif

/**
 * This file is part of input-overlay
 * which is licenced under the MIT licence.
 * See LICENCE or https://mit-license.org
 * github.com/univrsal/input-overlay
 */

extern uint16_t pressed_keys[MAX_KEYBOARD_KEYS];
extern bool pressed_pad_keys[PAD_COUNT][PAD_BUTTON_COUNT];
extern wint_t last_character;
extern int16_t mouse_x, mouse_y, mouse_x_smooth, mouse_y_smooth, mouse_last_x, mouse_last_y;
extern bool hook_initialized;

bool util_pressed_empty(void);
bool util_key_exists(uint16_t vc);
void util_clear_pressed(void);
void util_add_pressed(uint16_t vc);
void util_remove_pressed(uint16_t vc);
void util_set_pad_state(uint16_t btn_id, uint8_t id, bool state);

#ifdef WINDOWS
DWORD WINAPI hook_thread_proc(LPVOID arg);
#else
void *hook_thread_proc(void *arg);
#endif

void dispatch_proc(uiohook_event * const event);

bool logger_proc(unsigned int level, const char *format, ...);

void start_hook(void);

void end_hook(void);

int hook_enable(void);

void proccess_event(uiohook_event * const event);

#endif // HOOK_HELPER_HPP