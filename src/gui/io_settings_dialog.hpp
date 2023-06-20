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

#pragma once

#include <QDialog>
#include <mutex>
#include <memory>

namespace gamepad {
namespace cfg {
class binding;
}
class device;
}

namespace Ui {
class io_config_dialog;
}
class io_settings_dialog : public QDialog {
    Q_OBJECT

public:
    explicit io_settings_dialog(QWidget *parent = nullptr);

    ~io_settings_dialog();

    void showEvent(QShowEvent *event) override;

    void toggleShowHide();

private Q_SLOTS:

    void FormAccepted();

    void CbInputControlStateChanged(int state);

    void CbWssStateChanged(int state);

    void RefreshWindowList();

    void AddFilter();

    void RemoveFilter();

    static void OpenGitHub();

    static void OpenForums();

private:
    Ui::io_config_dialog *ui;
    QTimer *m_refresh = nullptr;
};

extern io_settings_dialog *settings_dialog;
