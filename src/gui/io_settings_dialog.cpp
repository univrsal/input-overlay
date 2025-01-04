/*************************************************************************
 * This file is part of input-overlay
 * git.vrsal.xyz/alex/input-overlay
 * Copyright 2023 univrsal <uni@vrsal.xyz>.
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
#include "../network/remote_connection.hpp"
#include "ui_io_settings_dialog.h"
#include "../util/config.hpp"
#include "../util/lang.h"
#include "../util/obs_util.hpp"
#include "../plugin-macros.generated.h"
#include <QDesktopServices>
#include <QTimer>
#include <QPair>
#include <obs-module.h>
#include <string>
#include <QMessageBox>

#ifndef GIT_COMMIT_HASH
#define GIT_COMMIT_HASH "unknown"
#endif

io_settings_dialog *settings_dialog = nullptr;

io_settings_dialog::io_settings_dialog(QWidget *parent) : QDialog(parent, Qt::Dialog), ui(new Ui::io_config_dialog)
{
    ui->setupUi(this);

    auto about = ui->txt_about->toHtml();
    about = about.replace("%version%", PLUGIN_VERSION);
    about = about.replace("%commit%", GIT_COMMIT_HASH);
    about = about.replace("%branch%", GIT_BRANCH);
    about = about.replace("%buildtime%", BUILD_TIME);
    ui->txt_about->setHtml(about);

    /* Connect QSlots */
    connect(ui->btn_github, &QPushButton::clicked, this, &io_settings_dialog::OpenGitHub);
    connect(ui->btn_forums, &QPushButton::clicked, this, &io_settings_dialog::OpenForums);
    connect(ui->button_box, &QDialogButtonBox::accepted, this, &io_settings_dialog::FormAccepted);
    connect(ui->cb_enable_control, &QCheckBox::stateChanged, this, &io_settings_dialog::CbInputControlStateChanged);
    connect(ui->btn_refresh_cb, &QPushButton::clicked, this, &io_settings_dialog::RefreshWindowList);
    connect(ui->btn_add, &QPushButton::clicked, this, &io_settings_dialog::AddFilter);
    connect(ui->btn_remove, &QPushButton::clicked, this, &io_settings_dialog::RemoveFilter);
    connect(ui->cb_enable_wss, &QCheckBox::stateChanged, this, &io_settings_dialog::CbWssStateChanged);

    /* Load values */
    ui->cb_iohook->setChecked(io_config::enable_uiohook);
    ui->cb_gamepad_hook->setChecked(io_config::enable_gamepad_hook);
    ui->cb_enable_overlay->setChecked(io_config::enable_overlay_source);
    ui->cb_enable_control->setChecked(io_config::enable_input_control);
    ui->cb_enable_wss->setChecked(io_config::enable_websocket_server);
    ui->cb_log->setChecked(io_config::log_flag);
    ui->txt_bind_address->setText(utf8_to_qt(io_config::wss_bind_address.c_str()));

    ui->tab_remote->hide(); // TODO: Redo protocol and update client to sdl2

    CbWssStateChanged(io_config::enable_websocket_server);
    CbInputControlStateChanged(io_config::enable_input_control);

    /* Display local ip for convenience */
    auto text = ui->lbl_status->text();
    text.append(network::get_local_ip());
    ui->lbl_status->setText(text);

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
}

void io_settings_dialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
}

void io_settings_dialog::toggleShowHide()
{
    setVisible(!isVisible());
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

    io_config::log_flag = ui->cb_log->isChecked();
    io_config::enable_input_control = ui->cb_enable_control->isChecked();
    io_config::filter_mode = ui->cb_list_mode->currentIndex();

    io_config::io_window_filters.set_regex(ui->cb_regex->isChecked());
    io_config::io_window_filters.set_whitelist(ui->cb_list_mode->currentIndex() == 0);
    io_config::io_window_filters.write_to_config();

    io_config::enable_websocket_server = ui->cb_enable_wss->isChecked();
    io_config::wss_bind_address = qt_to_utf8(ui->txt_bind_address->text());
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

void io_settings_dialog::CbWssStateChanged(int state)
{
    ui->sb_wss_port->setEnabled(state);
    ui->cb_log->setEnabled(state);
    ui->txt_bind_address->setEnabled(state);
}
