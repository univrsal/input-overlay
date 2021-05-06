#include "websocket_server.hpp"
#include "../util/config.hpp"
#include "../util/settings.h"
#include "../util/obs_util.hpp"
#include "mg.hpp"
#include <QJsonObject>
#include <QJsonDocument>

namespace wss {
bool start()
{
    const auto port = CGET_INT(S_WSS_PORT);
    std::string url = "ws://localhost:";
    url = url.append(std::to_string(port));
    return mg::start(url);
}

void stop()
{
    mg::stop();
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

void dispatch_gamepad_event(const gamepad::input_event *e, const std::shared_ptr<gamepad::device> &device, bool is_axis,
                            const std::string &source_name)
{
    std::lock_guard<std::mutex> lock(mg::poll_mutex);
    if (!mg::can_queue_message())
        return;
    QJsonObject obj;
    obj["event_source"] = source_name.c_str();
    obj["event_type"] = is_axis ? "gamepad_axis" : "gamepad_button";
    obj["device_name"] = utf8_to_qt(device->get_id().c_str());
    obj["device_index"] = device->get_index();
    obj["time"] = int(e->time);
    obj["virtual_code"] = e->vc;
    obj["virtual_value"] = e->virtual_value;
    obj["native_code"] = e->native_id;
    obj["native_value"] = e->value;

    QJsonDocument doc(obj);
    QString str(doc.toJson(QJsonDocument::Compact));

    mg::queue_message(qt_to_utf8(str));
}

void dispatch_gamepad_event(const std::shared_ptr<gamepad::device> &device, const char *state,
                            const std::string &source_name)
{
    std::lock_guard<std::mutex> lock(mg::poll_mutex);
    if (!mg::can_queue_message())
        return;
    QJsonObject obj;
    obj["event_source"] = source_name.c_str();
    obj["event_type"] = state;
    obj["device_name"] = utf8_to_qt(device->get_id().c_str());
    obj["time"] = int(gamepad::hook::ms_ticks());

    QJsonDocument doc(obj);
    QString str(doc.toJson(QJsonDocument::Compact));

    mg::queue_message(qt_to_utf8(str));
}

}