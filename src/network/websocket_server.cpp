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
    const auto port = CGET_INT(S_WSS_PORT);
    std::string url = "ws://localhost:";
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
    auto ev_to_str = [](int e) {
        switch (e) {
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
        obj["char"] = QString(e->data.keyboard.keychar);
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
        obj["clicks"] = e->data.wheel.clicks;
        obj["type"] = e->data.wheel.type;
        obj["amount"] = e->data.wheel.amount;
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
    QJsonObject obj;
    obj["event_source"] = source_name.c_str();
    obj["device_index"] = e->cdevice.which;
    obj["time"] = int(e->cdevice.timestamp);

    switch (e->type) {
    case SDL_CONTROLLERDEVICEADDED:
        break;

    case SDL_CONTROLLERDEVICEREMOVED:
        break;

    case SDL_CONTROLLERAXISMOTION:
        obj["event_type"] = "controller_axis_motion";
        obj["virtual_code"] = e->caxis.axis;
        obj["virtual_value"] = e->caxis.value / float(INT16_MAX);
        break;
    case SDL_CONTROLLERBUTTONDOWN:
        obj["event_type"] = "controller_button_down";
        obj["virtual_code"] = e->cbutton.button;
        obj["virtual_value"] = e->cbutton.state;
        break;
    case SDL_CONTROLLERBUTTONUP:
        obj["event_type"] = "controller_button_up";
        obj["virtual_code"] = e->cbutton.button;
        obj["virtual_value"] = e->cbutton.state;
        break;
    }

    auto n = data->remote_gamepad_names.find(e->cdevice.which);
    if (n != data->remote_gamepad_names.end())
        obj["device_name"] = utf8_to_qt(n->second.c_str());
    QJsonDocument doc(obj);
    auto j = QString(doc.toJson(QJsonDocument::Compact));
    std::lock_guard<std::mutex> lock(mg::poll_mutex);
    mg::queue_message(qt_to_utf8(j));
}

}
