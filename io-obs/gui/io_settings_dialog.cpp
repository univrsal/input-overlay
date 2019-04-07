/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "io_settings_dialog.hpp"
#include "network/remote_connection.hpp"
#include "network/io_server.hpp"
#include "util/util.hpp"
#include <obs-frontend-api.h>
#include <util/platform.h>
#include <util/config-file.h>
#include <hook/gamepad_hook.hpp>

namespace io_config
{
    input_filter io_window_filters; /* Global filters */
    std::mutex filter_mutex;        /* Thread safety for writing/reading filters */
}

io_settings_dialog::io_settings_dialog(QWidget* parent) : QDialog(parent, Qt::Dialog), ui(new Ui::io_config_dialog)
{
    const auto cfg = obs_frontend_get_global_config();
    ui->setupUi(this);

#ifndef LINUX
    ui->tab_gamepad->setVisible(false);
#else
    for (const auto& binding : gamepad::default_bindings) {
        auto text_box = findChild<QLineEdit*>(binding.text_box_id);
        if (text_box) {
            text_box->setText(QString::number(config_get_int(cfg, S_REGION, binding.setting)));
        }
    }
#endif

    connect(ui->button_box, &QDialogButtonBox::accepted, this, &io_settings_dialog::FormAccepted);

    connect(ui->cb_enable_remote, &QCheckBox::stateChanged, this, &io_settings_dialog::CbRemoteStateChanged);

    connect(ui->btn_refresh, &QPushButton::clicked, this, &io_settings_dialog::PingClients);

    connect(ui->cb_enable_control, &QCheckBox::stateChanged, this, &io_settings_dialog::CbInputControlStateChanged);
    connect(ui->btn_refresh_cb, &QPushButton::clicked, this, &io_settings_dialog::RefreshWindowList);
    connect(ui->btn_add, &QPushButton::clicked, this, &io_settings_dialog::AddFilter);
    connect(ui->btn_remove, &QPushButton::clicked, this, &io_settings_dialog::RemoveFilter);

    /* Load values */
    ui->cb_iohook->setChecked(config_get_bool(cfg, S_REGION, S_IOHOOK));
    ui->cb_gamepad_hook->setChecked(config_get_bool(cfg, S_REGION, S_GAMEPAD));
    ui->cb_enable_overlay->setChecked(config_get_bool(cfg, S_REGION, S_OVERLAY));
    ui->cb_enable_history->setChecked(config_get_bool(cfg, S_REGION, S_HISTORY));
    ui->cb_enable_control->setChecked(config_get_bool(cfg, S_REGION, S_CONTROL));
    ui->cb_enable_remote->setChecked(config_get_bool(cfg, S_REGION, S_REMOTE));
    ui->cb_log->setChecked(config_get_bool(cfg, S_REGION, S_LOGGING));
    ui->box_port->setValue(config_get_int(cfg, S_REGION, S_PORT));
    ui->cb_regex->setChecked(config_get_bool(cfg, S_REGION, S_REGEX));

    /* Tooltips aren't translated by obs */
    ui->box_refresh_rate->setToolTip(T_REFRESH_RATE_TOOLTIP);
    ui->lbl_refresh_rate->setToolTip(T_REFRESH_RATE_TOOLTIP);

    CbRemoteStateChanged(config_get_bool(cfg, S_REGION, S_REMOTE));
    CbInputControlStateChanged(config_get_bool(cfg, S_REGION, S_CONTROL));

    auto text = ui->lbl_status->text().toStdString();
    auto pos = text.find("%s");
    if (pos != std::string::npos)
        text.replace(pos, strlen("%s"), network::get_status());
    pos = text.find("%s");
    if (pos != std::string::npos)
        text.replace(pos, strlen("%s"), network::local_ip);
    ui->lbl_status->setText(text.c_str());


    /* Check for new connections every 250ms */
    m_refresh = new QTimer(this);
    connect(m_refresh, SIGNAL(timeout()), SLOT(RefreshUi()));
    m_refresh->start(250);

    /* Add current open windows to filter list */
    RefreshWindowList();

    for (const auto &filter : io_config::io_window_filters.filters()) {
        ui->list_filters->addItem(filter);
    }
}

void io_settings_dialog::showEvent(QShowEvent* event)
{
    Q_UNUSED(event)
    RefreshUi();
}

void io_settings_dialog::toggleShowHide()
{
    setVisible(!isVisible());
}

void io_settings_dialog::RefreshUi()
{
    /* Populate client list */
    if (network::network_flag && network::server_instance && network::server_instance->clients_changed()) {
        ui->box_connections->clear();
        QStringList list;
        std::vector<const char*> names;
        /* I'd do it differently, but including Qt headers and obs headers
         * creates conflicts with LOG_WARNING...
         */
        network::server_instance->get_clients(names);

        for (auto &name : names)
            list.append(name);
        ui->box_connections->addItems(list);
    }

#ifdef LINUX
    if (gamepad::last_input != 0xff) {
        auto mylineEdits = this->findChildren<QWidget*>();
        QListIterator<QWidget*> it(mylineEdits);
        QWidget* lineEditField;
        while (it.hasNext()) {
            lineEditField = it.next();
            if (auto lineE = qobject_cast<QLineEdit*>(lineEditField)) {
                if (lineE->hasFocus()) {
                    /* Set the binding of this textbox */
                    lineE->setText(QString::number(gamepad::last_input));
                    gamepad::last_input = 0xff;
                    break;
                }
            }
        }
    }
#endif
}

void io_settings_dialog::CbRemoteStateChanged(int state)
{
    ui->cb_log->setEnabled(state);
    ui->box_port->setEnabled(state);
    ui->box_connections->setEnabled(state);
    ui->btn_refresh->setEnabled(state);
    ui->box_refresh_rate->setEnabled(state);
    ui->cb_regex->setEnabled(state);
}

void io_settings_dialog::CbInputControlStateChanged(int state)
{
    ui->cb_list_mode->setEnabled(state);
    ui->cb_text->setEnabled(state);
    ui->btn_add->setEnabled(state);
    ui->btn_remove->setEnabled(state);
    ui->list_filters->setEnabled(state);
    ui->btn_refresh_cb->setEnabled(state);
}

void io_settings_dialog::PingClients()
{
    network::server_instance->ping_clients();
}

void io_settings_dialog::RefreshWindowList()
{
    std::vector<std::string> windows;
    GetWindowList(windows);
    ui->cb_text->clear();

    for (const auto &window : windows)
        ui->cb_text->addItem(window.c_str());
}

void io_settings_dialog::AddFilter()
{
    for (int i = 0; i < ui->list_filters->count(); i++)
        if (ui->list_filters->itemAt(0, i)->text() == ui->cb_text->currentText())
            return; /* Filter already exists */

    ui->list_filters->addItem(ui->cb_text->currentText());
    io_config::io_window_filters.add_filter(ui->cb_text->currentText().toStdString().c_str());
}

void io_settings_dialog::RemoveFilter()
{
    if (!ui->list_filters->selectedItems().empty()) {
        auto selected_count = ui->list_filters->selectedItems().size();
        for (int i = 0; i < selected_count; i++)
            io_config::io_window_filters.remove_filter(i);

        qDeleteAll(ui->list_filters->selectedItems());
    }
}

input_filter::~input_filter()
{
    m_filters.clear();
}

void io_settings_dialog::FormAccepted()
{
    auto cfg = obs_frontend_get_global_config();

    config_set_bool(cfg, S_REGION, S_IOHOOK, ui->cb_iohook->isChecked());
    config_set_bool(cfg, S_REGION, S_GAMEPAD, ui->cb_gamepad_hook->isChecked());
    config_set_bool(cfg, S_REGION, S_OVERLAY, ui->cb_enable_overlay->isChecked());
    config_set_bool(cfg, S_REGION, S_HISTORY, ui->cb_enable_history->isChecked());

    config_set_bool(cfg, S_REGION, S_REMOTE, ui->cb_enable_remote->isChecked());
    config_set_bool(cfg, S_REGION, S_LOGGING, ui->cb_log->isChecked());
    config_set_int(cfg, S_REGION, S_PORT, ui->box_port->value());

    config_set_bool(cfg, S_REGION, S_CONTROL, ui->cb_enable_control->isChecked());
    config_set_int(cfg, S_REGION, S_CONTROL_MODE, ui->cb_list_mode->currentIndex());

    io_config::io_window_filters.set_regex(ui->cb_regex->isChecked());
    io_config::io_window_filters.set_whitelist(ui->cb_list_mode->currentIndex() == 0);
    io_config::io_window_filters.write_to_config(cfg);

#ifdef LINUX
    for (const auto& binding : gamepad::default_bindings) {
        auto text_box = findChild<QLineEdit*>(binding.text_box_id);
        if (text_box) {
            bool ok = false;
            int value = text_box->text().toInt(&ok, 10);
            if (ok)
                config_set_int(cfg, S_REGION, binding.setting, value);
        }
    }
#endif
}


io_settings_dialog::~io_settings_dialog()
{
    delete ui;
    m_refresh->stop();
    delete m_refresh;
}

/* === input_filter === */
static std::string base_id = S_FILTER_BASE;

void input_filter::read_from_config(config_t* cfg)
{
    io_config::filter_mutex.lock();
    m_regex = config_get_bool(cfg, S_REGION, S_REGEX);
    m_whitelist = config_get_int(cfg, S_REGION, S_CONTROL_MODE) == 0;
    const int filter_count = config_get_int(cfg, S_REGION, S_FILTER_COUNT);

    for (auto i = 0; i < filter_count; ++i) {
        const auto str = config_get_string(cfg, S_REGION, (base_id + std::to_string(i)).c_str());
        if (str && strlen(str) > 0)
            m_filters.append(str);
    }
    io_config::filter_mutex.unlock();
}

void input_filter::write_to_config(config_t* cfg)
{
    io_config::filter_mutex.lock();

    config_set_int(cfg, S_REGION, S_FILTER_COUNT, m_filters.size());

    for (auto i = 0; i < m_filters.size(); i++) {
        config_set_string(cfg, S_REGION, (base_id + std::to_string(i)).c_str(), m_filters[i].toStdString().c_str());
    }

    io_config::filter_mutex.unlock();
}

void input_filter::add_filter(const char* filter)
{
    io_config::filter_mutex.lock();
    m_filters.append(filter);
    io_config::filter_mutex.unlock();
}

void input_filter::remove_filter(const int index)
{
    if (index >= 0 && index < m_filters.size())
        m_filters.removeAt(index);
}

void input_filter::set_regex(bool enabled)
{
    m_regex = enabled;
}

void input_filter::set_whitelist(bool wl)
{
    m_whitelist = wl;
}

bool input_filter::input_blocked()
{
    io_config::filter_mutex.lock();
    std::string current_window;
    auto flag = m_whitelist;
    GetCurrentWindowTitle(current_window);
    const char* window_str = current_window.c_str();

    for (const auto &filter : m_filters) {
        if (filter == window_str) {
            flag = !m_whitelist;
            break;
        }

        if (m_regex) {
            QRegExp regex(filter);
            if (regex.isValid() && regex.exactMatch(current_window.c_str())) {
                flag = !m_whitelist;
                break;
            }
        }
    }
    io_config::filter_mutex.unlock();
    return flag;
}

QStringList &input_filter::filters()
{
    return m_filters;
}
