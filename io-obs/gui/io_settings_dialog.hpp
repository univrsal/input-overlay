/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "ui_settings_dialog.hpp"
#include <QTimer>

class io_settings_dialog : public QDialog
{
Q_OBJECT

public:
    explicit io_settings_dialog(QWidget* parent = nullptr);
    ~io_settings_dialog();

    void showEvent(QShowEvent* event) override;
    void toggleShowHide();
	
private Q_SLOTS:
    void RefreshConnectionsList();
    void FormAccepted();
	void CbRemoteStateChanged(int state);
    void CbInputControlStateChanged(int state);
    void PingClients();
    void BoxRefreshChanged(int value);
    
private:
    Ui::io_config_dialog* ui;
	QTimer* m_refresh;
};

static io_settings_dialog* settings_dialog = nullptr;
