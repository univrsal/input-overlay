/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "ui_settings_dialog.hpp"
#include <QTimer>
#include <mutex>

typedef struct config_data config_t;

class input_filter
{
    QStringList m_filters;
    bool m_regex = false;
    bool m_whitelist = false;
public:
    ~input_filter();

    void read_from_config(config_t* cfg);

    void write_to_config(config_t* cfg);

    void add_filter(const char* filter);

    void remove_filter(int index);

    void set_regex(bool enabled);

    void set_whitelist(bool wl);

    bool input_blocked();

    QStringList& filters();
};


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
private:
    Ui::io_config_dialog* ui;
    QTimer* m_refresh;

};

static io_settings_dialog* settings_dialog = nullptr;

namespace io_config {
    extern input_filter io_window_filters;
    extern std::mutex filter_mutex;
}