/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */


#include "io_settings_dialog.hpp"
#include "network/remote_connection.hpp"
#include "network/io_server.hpp"
#include "util/util.hpp"
#include "util/config.hpp"
#include "hook/gamepad_hook.hpp"
#include <obs-frontend-api.h>
#include <util/platform.h>
#include <util/config-file.h>
#include <string>
#include <obs-module.h>
#include <QDesktopServices>

io_settings_dialog::io_settings_dialog(QWidget* parent) : QDialog(parent, Qt::Dialog), ui(new Ui::io_config_dialog)
{
    const auto cfg = obs_frontend_get_global_config();
    ui->setupUi(this);

#ifndef LINUX
    ui->tab_gamepad->setVisible(false);
#else
    for (const auto &binding : gamepad::default_bindings) {
        auto text_box = findChild<QLineEdit*>(binding.text_box_id);
        if (text_box) {
            text_box->setText(QString::number(config_get_int(cfg, S_REGION, binding.setting)));
        }
    }
#endif
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

    /* Load values */
    ui->cb_iohook->setChecked(io_config::uiohook);
    ui->cb_gamepad_hook->setChecked(io_config::gamepad);
    ui->cb_enable_overlay->setChecked(io_config::overlay);
    ui->cb_enable_history->setChecked(io_config::history);
    ui->cb_enable_control->setChecked(io_config::control);
    ui->cb_enable_remote->setChecked(io_config::remote);
    ui->cb_log->setChecked(io_config::log_flag);
    ui->box_port->setValue(io_config::port);
    ui->cb_regex->setChecked(io_config::regex);

    /* Tooltips aren't translated by obs */
    ui->box_refresh_rate->setToolTip(T_REFRESH_RATE_TOOLTIP);
    ui->lbl_refresh_rate->setToolTip(T_REFRESH_RATE_TOOLTIP);

    CbRemoteStateChanged(io_config::remote);
    CbInputControlStateChanged(io_config::control);

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
    if (io_config::control)
        RefreshWindowList();

    for (const auto &filter : io_config::io_window_filters.filters()) {
        ui->list_filters->addItem(filter);
    }
}

void io_settings_dialog::showEvent(QShowEvent* event)
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
    if (network::network_flag && network::server_instance && network::server_instance->clients_changed()) {
        ui->box_connections->clear();
        QStringList list;
        std::vector<const char*> names;
        /* I'd do it differently, but including Qt headers and obs headers
         * creates conflicts with LOG_WARNING...
         */
        network::server_instance->get_clients(names);

        for (auto &name : names)
            list.append(name);
        ui->box_connections->addItems(list);
    }

#ifdef LINUX
    if (gamepad::last_input != 0xff) {
        auto mylineEdits = this->findChildren<QWidget*>();
        QListIterator<QWidget*> it(mylineEdits);
        QWidget* lineEditField;
        while (it.hasNext()) {
            lineEditField = it.next();
            if (auto lineE = qobject_cast<QLineEdit*>(lineEditField)) {
                if (lineE->hasFocus()) {
                    /* Set the binding of this textbox */
                    lineE->setText(QString::number(gamepad::last_input));
                    gamepad::last_input = 0xff;
                    break;
                }
            }
        }
    }
#endif
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
    auto cfg = obs_frontend_get_global_config();

    io_config::uiohook = ui->cb_iohook->isChecked();
    io_config::gamepad = ui->cb_gamepad_hook->isChecked();
    io_config::overlay = ui->cb_enable_overlay->isChecked();
    io_config::history = ui->cb_enable_history->isChecked();

    io_config::remote = ui->cb_enable_remote->isChecked();
    io_config::log_flag = ui->cb_log->isChecked();
    io_config::port = ui->box_port->value();

    io_config::control = ui->cb_enable_control->isChecked();
    io_config::filter_mode = ui->cb_list_mode->currentIndex();

    io_config::io_window_filters.set_regex(ui->cb_regex->isChecked());
    io_config::io_window_filters.set_whitelist(ui->cb_list_mode->currentIndex() == 0);
    io_config::io_window_filters.write_to_config(cfg);

#ifdef LINUX
    for (const auto &binding : gamepad::default_bindings) {
        auto text_box = findChild<QLineEdit*>(binding.text_box_id);
        if (text_box) {
            bool ok = false;
            int value = text_box->text().toInt(&ok, 10);
            if (ok) {
                config_set_int(cfg, S_REGION, binding.setting, value);
                gamepad::bindings.set_binding(binding.default_value, value, binding.axis_event);
            }
        }
    }
#endif
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
