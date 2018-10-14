/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "ui_settings_dialog.hpp"

/* Settings values*/
#define S_REGION    "input-overlay"
#define S_IOHOOK    "iohook"
#define S_GAMEPAD   "gamepad"
#define S_OVERLAY   "overlay"
#define S_HISTORY   "history"
#define S_REMOTE    "remote"
#define S_LOGGING   "logging"
#define S_PORT      "port"

class io_settings_dialog : public QDialog
{
Q_OBJECT

public:
    explicit io_settings_dialog(QWidget* parent = nullptr);
    ~io_settings_dialog();

    void showEvent(QShowEvent* event) override;
    void toggleShowHide();
private Q_SLOTS:
    void FormAccepted();
	void CbRemoteStateChanged(int state);

private:
    Ui::io_config_dialog* ui;
};
