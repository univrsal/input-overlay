/*************************************************************************
 * This file is part of input-overlay
 * git.vrsal.cc/alex/input-overlay
 * Copyright 2025 univrsal <uni@vrsal.xyz>.
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

#include "websocket_server.hpp"
#include "../util/config.hpp"
#include "../util/settings.h"
#include "../util/obs_util.hpp"
#include "../util/log.h"
#include "mg.hpp"
#include <input_data.hpp>
#include <QJsonObject>
#include <QJsonDocument>

namespace wss {

std::atomic<bool> state = false;

bool start()
{
    if (state)
        return true;
    const auto port = CGET_INT(S_WSS_PORT);
    std::string url = "ws://" + io_config::wss_bind_address + ":";
    url = url.append(std::to_string(port));
    auto result = mg::start(url);
    if (result) {
        binfo("Starting websocket server on localhost:%li", port);
        state = true;
    }
    return result;
}

void stop()
{
    mg::stop();
    state = false;
}

QString serialize_uiohook(const uiohook_event *e, const std::string &source_name)
{
    QJsonObject obj;
    auto ev_to_str = [](int ev_type) {
        switch (ev_type) {
        case EVENT_KEY_TYPED:
            return "key_typed";
        case EVENT_KEY_PRESSED:
            return "key_pressed";
        case EVENT_KEY_RELEASED:
            return "key_released";
        case EVENT_MOUSE_CLICKED:
            return "mouse_clicked";
        case EVENT_MOUSE_PRESSED:
            return "mouse_pressed";
        case EVENT_MOUSE_RELEASED:
            return "mouse_released";
        case EVENT_MOUSE_MOVED:
            return "mouse_moved";
        case EVENT_MOUSE_DRAGGED:
            return "mouse_dragged";
        case EVENT_MOUSE_WHEEL:
            return "mouse_wheel";
        default:
            return "";
        }
    };

    switch (e->type) {
    case EVENT_KEY_TYPED:
        // convert wchar_t to QString
        obj["char"] = QString::fromWCharArray(&e->data.keyboard.keychar, 1);

        /* fallthrough */
    case EVENT_KEY_PRESSED:
    case EVENT_KEY_RELEASED:
        obj["event_source"] = utf8_to_qt(source_name.c_str());
        obj["event_type"] = QString(ev_to_str(e->type));
        obj["time"] = int(e->time);
        obj["mask"] = e->mask;
        obj["keycode"] = e->data.keyboard.keycode;
        obj["rawcode"] = e->data.keyboard.rawcode;
        break;
    case EVENT_MOUSE_CLICKED:
    case EVENT_MOUSE_PRESSED:
    case EVENT_MOUSE_RELEASED:
    case EVENT_MOUSE_MOVED:
    case EVENT_MOUSE_DRAGGED:
        obj["event_source"] = utf8_to_qt(source_name.c_str());
        obj["event_type"] = QString(ev_to_str(e->type));
        obj["time"] = int(e->time);
        obj["mask"] = e->mask;
        obj["button"] = e->data.mouse.button;
        obj["clicks"] = e->data.mouse.clicks;
        obj["x"] = e->data.mouse.x;
        obj["y"] = e->data.mouse.y;
        break;
    case EVENT_MOUSE_WHEEL:
        obj["event_source"] = utf8_to_qt(source_name.c_str());
        obj["event_type"] = QString(ev_to_str(e->type));
        obj["time"] = int(e->time);
        obj["mask"] = e->mask;
        obj["type"] = e->data.wheel.type;
        obj["delta"] = e->data.wheel.delta;
        obj["rotation"] = e->data.wheel.rotation;
        obj["direction"] = e->data.wheel.direction;
        obj["x"] = e->data.wheel.x;
        obj["y"] = e->data.wheel.y;
        break;
    default:;
    }
    QJsonDocument doc(obj);
    return QString(doc.toJson(QJsonDocument::Compact));
}

void dispatch_uiohook_event(const uiohook_event *e, const std::string &source_name)
{
    std::lock_guard<std::mutex> lock(mg::poll_mutex);
    if (mg::can_queue_message())
        mg::queue_message(qt_to_utf8(serialize_uiohook(e, source_name)));
}

void dispatch_sdl_event(const SDL_Event *e, const std::string &source_name, input_data *data)
{
    if (!mg::can_queue_message())
        return;
    static thread_local std::unordered_map<std::string, double[SDL_GAMEPAD_AXIS_COUNT]> last_axis;
    QJsonObject obj;
    obj["event_source"] = source_name.c_str();
    obj["device_index"] = (int)e->gdevice.which;
    obj["time"] = int(e->gdevice.timestamp);
    double axis{};
    switch (e->type) {
    case SDL_EVENT_GAMEPAD_ADDED:
        obj["event_type"] = "controller_device_added";
        obj["device_name"] = utf8_to_qt(SDL_GetGamepadNameForID(e->gdevice.which));
        break;

    case SDL_EVENT_GAMEPAD_REMOVED:
        obj["event_type"] = "controller_device_removed";
        // we don't have the name anymore
        break;
    case SDL_EVENT_GAMEPAD_AXIS_MOTION:
        // ignore small axis values
        axis = double(e->gaxis.value / double(INT16_MAX));
        if (std::abs(axis - last_axis[source_name][e->gaxis.axis]) < 0.003)
            return;
        last_axis[source_name][e->gaxis.axis] = axis;
        obj["event_type"] = "controller_axis_motion";
        obj["virtual_code"] = e->gaxis.axis;
        obj["virtual_value"] = axis;
        break;
    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
        obj["event_type"] = "controller_button_down";
        obj["virtual_code"] = e->gbutton.button;
        obj["virtual_value"] = e->gbutton.down;
        break;
    case SDL_EVENT_GAMEPAD_BUTTON_UP:
        obj["event_type"] = "controller_button_up";
        obj["virtual_code"] = e->gbutton.button;
        obj["virtual_value"] = e->gbutton.down;
        break;
    default:
        return; /* ignore other events */
    }

    auto n = data->remote_gamepad_names.find(e->gdevice.which);
    if (n != data->remote_gamepad_names.end())
        obj["device_name"] = utf8_to_qt(n->second.c_str());
    QJsonDocument doc(obj);
    auto j = QString(doc.toJson(QJsonDocument::Compact));
    std::lock_guard<std::mutex> lock(mg::poll_mutex);
    mg::queue_message(qt_to_utf8(j));
}

}
