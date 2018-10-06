/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 *
 * Referenced from obs-websocket
 * https://github.com/Palakis/obs-websocket/
 */

#pragma once

#include "settings_ui.hpp"

class settings_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit settings_dialog(QWidget* parent = nullptr);
    ~settings_dialog();

    void showEvent(QShowEvent* event) override;
    void ToggleShowHide();

private Q_SLOTS:
    void AuthCheckboxChanged();
    void FormAccepted();

    Ui::inptoverlayconfig* ui;
};
