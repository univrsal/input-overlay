#ifndef HOOK_HELPER_HPP
#define HOOK_HELPER_HPP

#define MAX_SIMULTANEOUS_KEYS 14

#include <stdint.h>
#include <uiohook.h>
#include "../util/util.hpp"

#ifdef WINDOWS
#include <Windows.h>
#endif

extern uint16_t pressed_keys[MAX_SIMULTANEOUS_KEYS];
extern int16_t mouse_x, mouse_y, mouse_x_smooth, mouse_y_smooth, mouse_last_x, mouse_last_y;
extern bool hook_initialized;

void util_clear_pressed(void);
bool util_pressed_empty(void);
bool util_key_exists(uint16_t vc);
void util_add_pressed(uint16_t vc);
void util_remove_pressed(uint16_t vc);

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