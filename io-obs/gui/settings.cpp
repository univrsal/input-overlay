/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 *
 * Referenced from obs-websocket
 * https://github.com/Palakis/obs-websocket/
 */

#include <obs-frontend-api.h>
#include "settings.hpp"

settings_dialog::settings_dialog(QWidget* parent) :
    QDialog(parent, Qt::Dialog),
    ui(new Ui::inptoverlayconfig)
{
    ui->setupUi(this);

    AuthCheckboxChanged();
}

void settings_dialog::showEvent(QShowEvent* event)
{

}

void settings_dialog::ToggleShowHide()
{
    if (!isVisible())
        setVisible(true);
    else
        setVisible(false);
}

void settings_dialog::AuthCheckboxChanged()
{

}

void settings_dialog::FormAccepted()
{

}

settings_dialog::~settings_dialog() {
    delete ui;
}
