/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "ui_settings_dialog.hpp"
#include <QTimer>
#include <mutex>
#include <netlib.h>

class io_settings_dialog : public QDialog
{
Q_OBJECT

public:
    explicit io_settings_dialog(QWidget* parent = nullptr);

    ~io_settings_dialog();

    void showEvent(QShowEvent* event) override;

    void toggleShowHide();

private Q_SLOTS:

    void RefreshUi();

    void FormAccepted();

    void CbRemoteStateChanged(int state);

    void CbInputControlStateChanged(int state);

    void PingClients();

    void RefreshWindowList();

    void AddFilter();

    void RemoveFilter();

    void OpenGitHub();

    void OpenForums();
private:
    Ui::io_config_dialog* ui;
    QTimer* m_refresh = nullptr;

};

static io_settings_dialog* settings_dialog = nullptr;