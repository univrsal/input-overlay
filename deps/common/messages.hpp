/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */
#pragma once

namespace network {
enum message : char {
    MSG_READ_ERROR = -2,
    MSG_INVALID,
    MSG_NAME_NOT_UNIQUE,
    MSG_NAME_INVALID,
    MSG_SERVER_SHUTDOWN,
    MSG_PING_CLIENT,
    MSG_UIOHOOK_EVENT,
    MSG_MOUSE_WHEEL_RESET,
    MSG_GAMEPAD_EVENT,
    MSG_GAMEPAD_CONNECTED,
    MSG_GAMEPAD_RECONNECTED,
    MSG_GAMEPAD_DISCONNECTED,
    MSG_CLIENT_DC,
    MSG_REFRESH,
    MSG_END_BUFFER,
    MSG_LAST
};
}
