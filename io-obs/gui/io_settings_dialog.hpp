/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */
 
#pragma once

#include "ui_settings_dialog.hpp"

class io_settings_dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit io_settings_dialog(QWidget* parent = 0);
    ~io_settings_dialog();
    
    void showEvent(QShowEvent* event);
    void toggleShowHide();
    
private Q_SLOTS:
    void FormAccepted();

private:
    Ui::io_config_dialog* ui;
};