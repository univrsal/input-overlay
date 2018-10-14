/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "io_settings_dialog.hpp"
#include <obs-frontend-api.h>
#include <util/config-file.h>

io_settings_dialog::io_settings_dialog(QWidget* parent)
    : QDialog(parent, Qt::Dialog),
    ui(new Ui::io_config_dialog)
{
    ui->setupUi(this);

    connect(ui->button_box, &QDialogButtonBox::accepted,
        this, &io_settings_dialog::FormAccepted);

	connect(ui->cb_enable_remote, &QCheckBox::stateChanged,
		this, &io_settings_dialog::CbRemoteStateChanged);

    auto cfg = obs_frontend_get_global_config();
    /* Set defaults */
    config_set_default_bool(cfg, S_REGION, S_IOHOOK, true);
    config_set_default_bool(cfg, S_REGION, S_GAMEPAD, true);
    config_set_default_bool(cfg, S_REGION, S_OVERLAY, true);
    config_set_default_bool(cfg, S_REGION, S_HISTORY, true);

    config_set_default_bool(cfg, S_REGION, S_REMOTE, false);
    config_set_default_bool(cfg, S_REGION, S_LOGGING, false);
    config_set_default_int(cfg, S_REGION, S_PORT, 1608);

    /* Load values */
    ui->cb_iohook->setChecked(config_get_bool(cfg, S_REGION, S_IOHOOK));
    ui->cb_gamepad_hook->setChecked(config_get_bool(cfg, S_REGION, S_GAMEPAD));
    ui->cb_enable_overlay->setChecked(config_get_bool(cfg, S_REGION, S_OVERLAY));
    ui->cb_enable_history->setChecked(config_get_bool(cfg, S_REGION, S_HISTORY));

    ui->cb_enable_remote->setChecked(config_get_bool(cfg, S_REGION, S_REMOTE));
    ui->cb_log->setChecked(config_get_bool(cfg, S_REGION, S_LOGGING));
    ui->box_port->setValue(config_get_int(cfg, S_REGION, S_PORT));

	CbRemoteStateChanged(0);
}

void io_settings_dialog::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);
}

void io_settings_dialog::toggleShowHide()
{
    setVisible(!isVisible());
}

void io_settings_dialog::CbRemoteStateChanged(int state)
{
	ui->cb_log->setEnabled(ui->cb_enable_remote->isChecked());
	ui->box_port->setEnabled(ui->cb_enable_remote->isChecked());
	ui->box_connections->setEnabled(ui->cb_enable_remote->isChecked());
}

void io_settings_dialog::FormAccepted()
{
    auto cfg = obs_frontend_get_global_config();
    config_set_bool(cfg, S_REGION, S_IOHOOK, ui->cb_iohook->isChecked());
    config_set_bool(cfg, S_REGION, S_GAMEPAD, ui->cb_gamepad_hook->isChecked());
    config_set_bool(cfg, S_REGION, S_OVERLAY, ui->cb_enable_overlay->isChecked());
    config_set_bool(cfg, S_REGION, S_HISTORY, ui->cb_enable_history->isChecked());

    config_set_bool(cfg, S_REGION, S_REMOTE, ui->cb_enable_remote->isChecked());
    config_set_bool(cfg, S_REGION, S_LOGGING, ui->cb_log->isChecked());
    config_set_int(cfg, S_REGION, S_PORT, ui->box_port->value());
}


io_settings_dialog::~io_settings_dialog()
{
    delete ui;
}
