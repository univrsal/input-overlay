/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2021 univrsal <uni@vrsal.de>.
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

#include "io_settings_dialog.hpp"
#include "../network/io_server.hpp"
#include "../network/remote_connection.hpp"
#include "ui_io_settings_dialog.h"
#include "../util/config.hpp"
#include "../util/lang.h"
#include "../util/obs_util.hpp"
#include "../hook/gamepad_hook_helper.hpp"
#include <libgamepad.hpp>
#include <QDesktopServices>
#include <QTimer>
#include <QPair>
#include <obs-module.h>
#include <string>
#include <QMessageBox>

static QPair<const char *, uint16_t> button_map[] = {{"txt_a", gamepad::button::A},
                                                     {"txt_b", gamepad::button::B},
                                                     {"txt_x", gamepad::button::X},
                                                     {"txt_y", gamepad::button::Y},
                                                     {"txt_rb", gamepad::button::RB},
                                                     {"txt_lb", gamepad::button::LB},
                                                     {"txt_guide", gamepad::button::GUIDE},
                                                     {"txt_analog_left", gamepad::button::L_THUMB},
                                                     {"txt_analog_right", gamepad::button::R_THUMB},
                                                     {"txt_back", gamepad::button::BACK},
                                                     {"txt_start", gamepad::button::START},
                                                     {"txt_dpad_up", gamepad::button::DPAD_UP},
                                                     {"txt_dpad_down", gamepad::button::DPAD_DOWN},
                                                     {"txt_dpad_left", gamepad::button::DPAD_LEFT},
                                                     {"txt_dpad_right", gamepad::button::DPAD_RIGHT}};
static QPair<const char *, uint16_t> axis_map[] = {
    {"txt_lx", gamepad::axis::LEFT_STICK_X},  {"txt_ly", gamepad::axis::LEFT_STICK_Y},
    {"txt_rx", gamepad::axis::RIGHT_STICK_X}, {"txt_ry", gamepad::axis::RIGHT_STICK_Y},
    {"txt_lt", gamepad::axis::LEFT_TRIGGER},  {"txt_rt", gamepad::axis::RIGHT_TRIGGER}};

io_settings_dialog *settings_dialog = nullptr;

io_settings_dialog::io_settings_dialog(QWidget *parent) : QDialog(parent, Qt::Dialog), ui(new Ui::io_config_dialog)
{
    ui->setupUi(this);

    /* Connect QSlots */
    connect(ui->btn_github, &QPushButton::clicked, this, &io_settings_dialog::OpenGitHub);
    connect(ui->btn_forums, &QPushButton::clicked, this, &io_settings_dialog::OpenForums);
    connect(ui->button_box, &QDialogButtonBox::accepted, this, &io_settings_dialog::FormAccepted);
    connect(ui->cb_enable_remote, &QCheckBox::stateChanged, this, &io_settings_dialog::CbRemoteStateChanged);
    connect(ui->btn_refresh, &QPushButton::clicked, this, &io_settings_dialog::PingClients);
    connect(ui->cb_enable_control, &QCheckBox::stateChanged, this, &io_settings_dialog::CbInputControlStateChanged);
    connect(ui->btn_refresh_cb, &QPushButton::clicked, this, &io_settings_dialog::RefreshWindowList);
    connect(ui->btn_add, &QPushButton::clicked, this, &io_settings_dialog::AddFilter);
    connect(ui->btn_remove, &QPushButton::clicked, this, &io_settings_dialog::RemoveFilter);
    connect(ui->cb_gamepad_hook, &QCheckBox::stateChanged, this, &io_settings_dialog::CbEnableGamepadChanged);
    connect(ui->cb_enable_wss, &QCheckBox::stateChanged, this, &io_settings_dialog::CbWssStateChanged);

    /* Load values */
    ui->cb_iohook->setChecked(io_config::enable_uiohook);
    ui->cb_gamepad_hook->setChecked(io_config::enable_gamepad_hook);
    ui->cb_enable_overlay->setChecked(io_config::enable_overlay_source);
    ui->cb_enable_control->setChecked(io_config::enable_input_control);
    ui->cb_enable_remote->setChecked(io_config::enable_remote_connections);
    ui->cb_enable_wss->setChecked(io_config::enable_websocket_server);
    ui->cb_log->setChecked(io_config::log_flag);
    ui->box_port->setValue(io_config::server_port);

    load_bindings();

    /* Tooltips aren't translated by obs */
    ui->box_refresh_rate->setToolTip(T_REFRESH_RATE_TOOLTIP);
    ui->lbl_refresh_rate->setToolTip(T_REFRESH_RATE_TOOLTIP);

    CbRemoteStateChanged(io_config::enable_remote_connections);
    CbInputControlStateChanged(io_config::enable_input_control);

    /* Sets up remote connection status label */
    auto text = ui->lbl_status->text().toStdString();
    auto pos = text.find("%s");
    if (pos != std::string::npos)
        text.replace(pos, strlen("%s"), network::get_status());
    pos = text.find("%s");
    if (pos != std::string::npos)
        text.replace(pos, strlen("%s"), network::local_ip);
    ui->lbl_status->setText(text.c_str());

    /* Check for new connections every 250ms */
    m_refresh = new QTimer(this);
    connect(m_refresh, SIGNAL(timeout()), SLOT(RefreshUi()));
    m_refresh->start(250);

    /* Add current open windows to filter list */
    if (io_config::enable_input_control)
        RefreshWindowList();

    for (const auto &filter : io_config::io_window_filters.filters()) {
        ui->list_filters->addItem(filter);
    }

    /* Set red color on label so people don't miss it */
    ui->lbl_local_features->setStyleSheet("QLabel { color: red; "
                                          "font-weight: bold;}");

    ui->rb_js->setEnabled(io_config::enable_gamepad_hook);
    ui->rb_by_id->setEnabled(io_config::enable_gamepad_hook);
    ui->rb_xinput->setEnabled(io_config::enable_gamepad_hook);
    ui->rb_dinput->setEnabled(io_config::enable_gamepad_hook);

#ifdef _WIN32
    ui->rb_dinput->setChecked(io_config::use_dinput);
    ui->rb_xinput->setChecked(!ui->rb_dinput->isChecked());
#endif

    // If we set these on all platforms they'll deselect the other radio buttons
#ifdef __linux__
    ui->rb_js->setChecked(io_config::use_js);
    ui->rb_by_id->setChecked(!ui->rb_js->isChecked());
#endif

#ifndef _WIN32
    ui->rb_dinput->setVisible(false);
    ui->rb_xinput->setVisible(false);
#endif
#ifndef __linux__
    ui->rb_js->setVisible(false);
    ui->rb_by_id->setVisible(false);
#endif

    // Load first binding to ui
    libgamepad::hook_instance->get_mutex()->lock();
    if (!libgamepad::hook_instance->get_bindings().empty())
        load_binding_to_ui(libgamepad::hook_instance->get_bindings()[0]);
    libgamepad::hook_instance->get_mutex()->unlock();
}

void io_settings_dialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    RefreshUi();
}

void io_settings_dialog::toggleShowHide()
{
    setVisible(!isVisible());
}

void io_settings_dialog::RefreshUi()
{
    /* Populate client list */
    if (network::network_flag && network::server_instance) {
        std::lock_guard<std::mutex> lock(network::mutex);
        if (network::server_instance->clients_changed()) {
            ui->box_connections->clear();
            QStringList list;
            std::vector<const char *> names;
            /* I'd do it differently, but including Qt headers and obs headers
         * creates conflicts with LOG_WARNING...
         */
            network::server_instance->get_clients(names);

            for (auto &name : names)
                list.append(name);
            ui->box_connections->addItems(list);
        }
    }

    if (libgamepad::state) {
        gamepad::device_list devs;
        {
            auto &mutex = *libgamepad::hook_instance->get_mutex();
            std::lock_guard<std::mutex> lock(mutex);
            devs = libgamepad::hook_instance->get_devices();
        }

        /* Fill device list */
        if (int(devs.size()) != ui->cb_device->count()) {
            auto selected = ui->cb_device->currentIndex();
            ui->cb_device->clear();
            for (const auto &dev : devs) {
                ui->cb_device->addItem(utf8_to_qt(dev->get_name().c_str()),
                                       QVariant::fromValue(utf8_to_qt(dev->get_id().c_str())));
            }
            ui->cb_device->setCurrentIndex(selected);
        }

        // Select something if nothing is selected
        if (ui->cb_device->currentText().isEmpty() && !devs.empty()) {
            ui->cb_device->setCurrentIndex(0);
        }

        libgamepad::last_input_mutex.lock();
        if (m_last_gamepad_input < libgamepad::last_input_time) {
            m_last_gamepad_input = libgamepad::last_input_time;
            auto mylineEdits = ui->scrollArea->findChildren<QWidget *>();
            QListIterator<QWidget *> it(mylineEdits);
            QWidget *lineEditField;
            while (it.hasNext()) {
                lineEditField = it.next();
                if (auto lineE = qobject_cast<QLineEdit *>(lineEditField)) {
                    if (lineE->hasFocus()) {
                        // LT and RT are the same axis in DInput, so we save the polarity here
                        if (libgamepad::flags & gamepad::hook_type::DIRECT_INPUT &&
                            (lineE->objectName() == "txt_lt" || lineE->objectName() == "txt_rt")) {
                            /* Set the binding of this textbox */
                            lineE->setText((libgamepad::last_input_value < 0 ? "-" : "+") +
                                           QString::number(libgamepad::last_input));
                        } else {
                            /* Set the binding of this textbox */
                            lineE->setText(QString::number(libgamepad::last_input));
                        }

                        break;
                    }
                }
            }
        }
        libgamepad::last_input_mutex.unlock();
    }
}

void io_settings_dialog::CbRemoteStateChanged(int state)
{
    ui->cb_log->setEnabled(state);
    ui->box_port->setEnabled(state);
    ui->box_connections->setEnabled(state);
    ui->btn_refresh->setEnabled(state);
    ui->box_refresh_rate->setEnabled(state);
    ui->cb_regex->setEnabled(state);
}

void io_settings_dialog::CbInputControlStateChanged(int state)
{
    ui->cb_list_mode->setEnabled(state);
    ui->cb_text->setEnabled(state);
    ui->btn_add->setEnabled(state);
    ui->btn_remove->setEnabled(state);
    ui->list_filters->setEnabled(state);
    ui->btn_refresh_cb->setEnabled(state);
    ui->cb_regex->setEnabled(state);
}

void io_settings_dialog::PingClients()
{
    network::server_instance->ping_clients();
}

void io_settings_dialog::RefreshWindowList()
{
    std::vector<std::string> windows;
    GetWindowList(windows);
    ui->cb_text->clear();

    for (const auto &window : windows)
        ui->cb_text->addItem(window.c_str());
}

void io_settings_dialog::AddFilter()
{
    for (int i = 0; i < ui->list_filters->count(); i++)
        if (ui->list_filters->itemAt(0, i)->text() == ui->cb_text->currentText())
            return; /* Filter already exists */

    ui->list_filters->addItem(ui->cb_text->currentText());
    io_config::io_window_filters.add_filter(ui->cb_text->currentText().toStdString().c_str());
}

void io_settings_dialog::RemoveFilter()
{
    if (!ui->list_filters->selectedItems().empty()) {
        auto selected_count = ui->list_filters->selectedItems().size();
        for (int i = 0; i < selected_count; i++)
            io_config::io_window_filters.remove_filter(i);

        qDeleteAll(ui->list_filters->selectedItems());
    }
}

void io_settings_dialog::FormAccepted()
{
    io_config::enable_uiohook = ui->cb_iohook->isChecked();
    io_config::enable_gamepad_hook = ui->cb_gamepad_hook->isChecked();
    io_config::enable_overlay_source = ui->cb_enable_overlay->isChecked();

    io_config::enable_remote_connections = ui->cb_enable_remote->isChecked();
    io_config::log_flag = ui->cb_log->isChecked();
    io_config::server_port = ui->box_port->value();

    io_config::enable_input_control = ui->cb_enable_control->isChecked();
    io_config::filter_mode = ui->cb_list_mode->currentIndex();

    io_config::io_window_filters.set_regex(ui->cb_regex->isChecked());
    io_config::io_window_filters.set_whitelist(ui->cb_list_mode->currentIndex() == 0);
    io_config::io_window_filters.write_to_config();

    io_config::enable_websocket_server = ui->cb_enable_wss->isChecked();
    io_config::use_dinput = ui->rb_dinput->isChecked();
    io_config::save();
}

io_settings_dialog::~io_settings_dialog()
{
    delete ui;
    m_refresh->stop();
    delete m_refresh;
}

void io_settings_dialog::OpenGitHub()
{
    QDesktopServices::openUrl(QUrl("https://github.com/univrsal/input-overlay"));
}

void io_settings_dialog::OpenForums()
{
    QDesktopServices::openUrl(QUrl("https://obsproject.com/forum/resources/input-overlay.552/"));
}

void io_settings_dialog::load_bindings()
{
    if (!libgamepad::hook_instance)
        return;
    gamepad::bindings_list bindings;
    {
        auto &mutex = *libgamepad::hook_instance->get_mutex();
        std::lock_guard<std::mutex> lock(mutex);
        bindings = libgamepad::hook_instance->get_bindings();
    }
    ui->cb_bindings->clear();
    for (const auto &binding : bindings) {
        ui->cb_bindings->addItem(utf8_to_qt(binding->get_name().c_str()));
    }
}

int find_by_code(const QPair<const char *, uint16_t> &needle, const gamepad::cfg::mappings &haystack)
{
    for (const auto &p : haystack) {
        if (p.second == needle.second)
            return p.first;
    }
    return -1;
}

void io_settings_dialog::load_binding_to_ui(const std::shared_ptr<gamepad::cfg::binding> &binding)
{
    auto dinput_binding = dynamic_cast<gamepad::cfg::binding_dinput *>(binding.get());

    for (const auto &pair : button_map) {
        // transfer binds to textboxes
        auto text_box = findChild<QLineEdit *>(pair.first);
        if (text_box) {
            auto native_value = find_by_code(pair, binding->get_button_mappings());
            if (native_value > -1)
                text_box->setText(QString::number(native_value));
        }
    }

    for (const auto &pair : axis_map) {
        // transfer binds to textboxes
        auto text_box = findChild<QLineEdit *>(pair.first);
        if (text_box) {
            auto native_value = find_by_code(pair, binding->get_axis_mappings());
            if (native_value > -1) {
                QString val = QString::number(native_value);

                // This is bad, but so is DirectInput
                if (dinput_binding) {
                    if (text_box->objectName() == "txt_lt")
                        val = val.prepend(dinput_binding->left_trigger_polarity() > 0 ? "+" : "-");
                    else if (text_box->objectName() == "txt_rt")
                        val = val.prepend(dinput_binding->right_trigger_polarity() > 0 ? "+" : "-");
                }
                text_box->setText(val);
            }
        }
    }

    /* Select it if the binding exists */
    auto idx = ui->cb_bindings->findText(utf8_to_qt(binding->get_name().c_str()));
    if (idx > -1)
        ui->cb_bindings->setCurrentIndex(idx);
}

void io_settings_dialog::on_btn_add_bind_clicked()
{
    if (ui->txt_new_binding_name->text().isEmpty())
        return;
    if (ui->cb_bindings->findText(ui->txt_new_binding_name->text()) != -1) {
        QMessageBox::information(this, "Error", "Binding name must be unique");
        return;
    }

    /* The callback locks the mutex so we cant lock it here */
    auto *mutex = libgamepad::hook_instance->get_mutex();

    auto dev = get_selected_device();
    if (dev) {
        mutex->lock();
        auto has_custom_binding = libgamepad::hook_instance->get_binding_by_name(dev->get_binding()->get_name()) !=
                                  nullptr;
        /* If this device has a custom set binding, we load it, otherwise
         * we keep whatever is already set by the user in the UI and set the binding
         * for the device
         */
        auto binding = libgamepad::hook_instance->make_native_binding();
        binding->set_name(qt_to_utf8(ui->txt_new_binding_name->text()));
        libgamepad::hook_instance->add_binding(binding);
        if (has_custom_binding)
            load_binding_from_ui(binding);
        mutex->unlock();
        ui->cb_bindings->addItem(ui->txt_new_binding_name->text());
    }
    ui->txt_new_binding_name->clear();
}

void io_settings_dialog::on_cb_device_currentIndexChanged(int)
{
    std::shared_ptr<gamepad::device> dev;
    {
        auto &mutex = *libgamepad::hook_instance->get_mutex();
        std::lock_guard<std::mutex> lock(mutex);
        dev = get_selected_device();
    }
    if (dev)
        load_binding_to_ui(dev->get_binding());
}

void io_settings_dialog::on_cb_bindings_currentIndexChanged(int)
{
    auto &mutex = *libgamepad::hook_instance->get_mutex();
    std::lock_guard<std::mutex> lock(mutex);
    auto binding = get_selected_binding();
    auto dev = get_selected_device();

    if (dev)
        dev->set_binding(binding);
    if (binding)
        load_binding_to_ui(binding);
}

void io_settings_dialog::on_box_binding_accepted()
{
    auto &mutex = *libgamepad::hook_instance->get_mutex();
    std::lock_guard<std::mutex> lock(mutex);
    auto binding = get_selected_binding();

    if (!binding) {
        /* No binding exists with this name so we create it */
        binding = libgamepad::hook_instance->make_native_binding();
        binding->set_name(qt_to_utf8(ui->cb_bindings->currentText()));
        libgamepad::hook_instance->add_binding(binding);
    }

    auto dinput_binding = dynamic_cast<gamepad::cfg::binding_dinput *>(binding.get());
    /* Read bindings from UI and set them to the current device bindings */
    for (const auto &pair : button_map) {
        // transfer binds from ui
        auto text_box = findChild<QLineEdit *>(pair.first);
        if (text_box) {
            bool ok = false;
            auto i = text_box->text().toInt(&ok);
            if (ok)
                binding->get_button_mappings()[i] = pair.second;
        }
    }

    for (const auto &pair : axis_map) {
        // transfer binds from ui
        auto text_box = findChild<QLineEdit *>(pair.first);
        if (text_box) {
            bool ok = false;
            auto i = abs(text_box->text().toInt(&ok)); // ignore -/+ used for direct input
            if (ok) {
                binding->get_axis_mappings()[i] = pair.second;

                // This is bad, but so is DirectInput
                if (dinput_binding) {
                    if (text_box->objectName() == "txt_lt")
                        dinput_binding->left_trigger_polarity() = text_box->text().contains("-") ? -1 : 1;
                    else if (text_box->objectName() == "txt_rt")
                        dinput_binding->right_trigger_polarity() = text_box->text().contains("-") ? -1 : 1;
                }
            }
        }
    }

    // This is usually already the case, since it'll be done if the selected
    // device changes or the selected binding changes, but if we only have
    // one device and one binding the comboboxes can't be changed so we do it
    // here as well
    if (auto dev = get_selected_device())
        dev->set_binding(binding);
}

std::shared_ptr<gamepad::device> io_settings_dialog::get_selected_device() const
{
    return libgamepad::hook_instance->get_device_by_id(qt_to_utf8(ui->cb_device->currentData().toString()));
}

std::shared_ptr<gamepad::cfg::binding> io_settings_dialog::get_selected_binding() const
{
    return libgamepad::hook_instance->get_binding_by_name(qt_to_utf8(ui->cb_bindings->currentText()));
}

void io_settings_dialog::CbEnableGamepadChanged(int)
{
    auto enabled = ui->cb_gamepad_hook->isChecked();
    ui->rb_dinput->setEnabled(enabled);
    ui->rb_xinput->setEnabled(enabled);
    ui->rb_js->setEnabled(enabled);
    ui->rb_by_id->setEnabled(enabled);
}

void io_settings_dialog::CbWssStateChanged(int state)
{
    ui->sb_wss_port->setEnabled(ui->cb_enable_wss->isChecked());
}

void io_settings_dialog::load_binding_from_ui(std::shared_ptr<gamepad::cfg::binding> binding)
{
    auto dinput_binding = dynamic_cast<gamepad::cfg::binding_dinput *>(binding.get());

    for (const auto &pair : button_map) {
        // transfer textbox value to bind
        auto text_box = findChild<QLineEdit *>(pair.first);
        if (text_box) {
            bool ok = false;
            auto i = abs(text_box->text().toInt(&ok)); // ignore -/+ used for direct input
            if (ok)
                binding->get_button_mappings()[i] = pair.second;
        }
    }

    for (const auto &pair : axis_map) {
        // transfer binds to textboxes
        auto text_box = findChild<QLineEdit *>(pair.first);
        if (text_box) {
            bool ok = false;
            auto i = text_box->text().toInt(&ok); // ignore -/+ used for direct input
            if (ok)
                binding->get_button_mappings()[abs(i)] = pair.second;

            if (dinput_binding) {
                if (text_box->objectName() == "txt_lt")
                    dinput_binding->left_trigger_polarity() = i > 0 ? 1 : -1;
                else if (text_box->objectName() == "txt_rt")
                    dinput_binding->right_trigger_polarity() = i > 0 ? 1 : -1;
            }
        }
    }
}
