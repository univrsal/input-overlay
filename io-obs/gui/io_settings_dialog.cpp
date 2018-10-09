/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "io_settings_dialog.hpp"

io_settings_dialog::io_settings_dialog(QWidget* parent)
    : QDialog(parent, Qt::Dialog),
    ui(new Ui::io_config_dialog)
{
    ui->setupUi(this);

    connect(ui->button_box, &QDialogButtonBox::accepted,
        this, &io_settings_dialog::FormAccepted);
}

void io_settings_dialog::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);
}

void io_settings_dialog::toggleShowHide()
{
    setVisible(!isVisible());
}

void io_settings_dialog::FormAccepted()
{
}

io_settings_dialog::~io_settings_dialog()
{
    delete ui;
}
