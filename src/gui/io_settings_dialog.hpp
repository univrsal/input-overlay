/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2022 univrsal <uni@vrsal.xyz>.
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

    void RefreshUi();

    void FormAccepted();

    void CbRemoteStateChanged(int state);

    void CbInputControlStateChanged(int state);

    void CbWssStateChanged(int state);

    void PingClients();

    void RefreshWindowList();

    void AddFilter();

    void CbEnableGamepadChanged(int state);

    void RemoveFilter();

    static void OpenGitHub();

    static void OpenForums();

    void on_btn_add_bind_clicked();

    void on_btn_remove_bind_clicked();

    void on_cb_device_currentIndexChanged(int index);

    void on_cb_bindings_currentIndexChanged(int index);

    void on_box_binding_accepted();

private:
    std::shared_ptr<gamepad::device> get_selected_device() const;
    std::shared_ptr<gamepad::cfg::binding> get_selected_binding() const;
    void load_bindings();
    void load_binding_to_ui(const std::shared_ptr<gamepad::cfg::binding> &binding);
    void load_binding_from_ui(std::shared_ptr<gamepad::cfg::binding> binding);
    uint64_t m_last_gamepad_input = 0;
    Ui::io_config_dialog *ui;
    QTimer *m_refresh = nullptr;
    std::vector<std::string> m_bindings_to_remove;
};

extern io_settings_dialog *settings_dialog;
