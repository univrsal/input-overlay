/********************************************************************************
** Form generated from reading UI file 'io_settings_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS_DIALOG_H
#define UI_SETTINGS_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_io_config_dialog
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *gb_features;
    QVBoxLayout *verticalLayout_2;
    QLabel *lbl_local_features;
    QCheckBox *cb_iohook;
    QCheckBox *cb_gamepad_hook;
    QCheckBox *cb_enable_overlay;
    QCheckBox *cb_enable_history;
    QGroupBox *gb_remote;
    QVBoxLayout *verticalLayout_3;
    QCheckBox *cb_enable_remote;
    QCheckBox *cb_log;
    QLabel *lbl_port;
    QSpinBox *box_port;
    QLabel *lbl_status;
    QLabel *lbl_connections;
    QListWidget *box_connections;
    QDialogButtonBox *button_box;

    void setupUi(QDialog *io_config_dialog)
    {
        if (io_config_dialog->objectName().isEmpty())
            io_config_dialog->setObjectName(QStringLiteral("io_config_dialog"));
        io_config_dialog->resize(340, 575);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(io_config_dialog->sizePolicy().hasHeightForWidth());
        io_config_dialog->setSizePolicy(sizePolicy);
        io_config_dialog->setMinimumSize(QSize(340, 575));
        verticalLayout = new QVBoxLayout(io_config_dialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        gb_features = new QGroupBox(io_config_dialog);
        gb_features->setObjectName(QStringLiteral("gb_features"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(6);
        sizePolicy1.setVerticalStretch(4);
        sizePolicy1.setHeightForWidth(gb_features->sizePolicy().hasHeightForWidth());
        gb_features->setSizePolicy(sizePolicy1);
        gb_features->setMinimumSize(QSize(0, 134));
        verticalLayout_2 = new QVBoxLayout(gb_features);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        lbl_local_features = new QLabel(gb_features);
        lbl_local_features->setObjectName(QStringLiteral("lbl_local_features"));

        verticalLayout_2->addWidget(lbl_local_features);

        cb_iohook = new QCheckBox(gb_features);
        cb_iohook->setObjectName(QStringLiteral("cb_iohook"));

        verticalLayout_2->addWidget(cb_iohook);

        cb_gamepad_hook = new QCheckBox(gb_features);
        cb_gamepad_hook->setObjectName(QStringLiteral("cb_gamepad_hook"));

        verticalLayout_2->addWidget(cb_gamepad_hook);

        cb_enable_overlay = new QCheckBox(gb_features);
        cb_enable_overlay->setObjectName(QStringLiteral("cb_enable_overlay"));

        verticalLayout_2->addWidget(cb_enable_overlay);

        cb_enable_history = new QCheckBox(gb_features);
        cb_enable_history->setObjectName(QStringLiteral("cb_enable_history"));

        verticalLayout_2->addWidget(cb_enable_history);


        verticalLayout->addWidget(gb_features);

        gb_remote = new QGroupBox(io_config_dialog);
        gb_remote->setObjectName(QStringLiteral("gb_remote"));
        gb_remote->setEnabled(true);
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(gb_remote->sizePolicy().hasHeightForWidth());
        gb_remote->setSizePolicy(sizePolicy2);
        verticalLayout_3 = new QVBoxLayout(gb_remote);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        cb_enable_remote = new QCheckBox(gb_remote);
        cb_enable_remote->setObjectName(QStringLiteral("cb_enable_remote"));

        verticalLayout_3->addWidget(cb_enable_remote);

        cb_log = new QCheckBox(gb_remote);
        cb_log->setObjectName(QStringLiteral("cb_log"));

        verticalLayout_3->addWidget(cb_log);

        lbl_port = new QLabel(gb_remote);
        lbl_port->setObjectName(QStringLiteral("lbl_port"));

        verticalLayout_3->addWidget(lbl_port);

        box_port = new QSpinBox(gb_remote);
        box_port->setObjectName(QStringLiteral("box_port"));
        box_port->setButtonSymbols(QAbstractSpinBox::NoButtons);
        box_port->setProperty("showGroupSeparator", QVariant(false));
        box_port->setMaximum(65535);
        box_port->setValue(1608);

        verticalLayout_3->addWidget(box_port);

        lbl_status = new QLabel(gb_remote);
        lbl_status->setObjectName(QStringLiteral("lbl_status"));

        verticalLayout_3->addWidget(lbl_status);

        lbl_connections = new QLabel(gb_remote);
        lbl_connections->setObjectName(QStringLiteral("lbl_connections"));

        verticalLayout_3->addWidget(lbl_connections);

        box_connections = new QListWidget(gb_remote);
        box_connections->setObjectName(QStringLiteral("box_connections"));
        sizePolicy2.setHeightForWidth(box_connections->sizePolicy().hasHeightForWidth());
        box_connections->setSizePolicy(sizePolicy2);

        verticalLayout_3->addWidget(box_connections);


        verticalLayout->addWidget(gb_remote);

        button_box = new QDialogButtonBox(io_config_dialog);
        button_box->setObjectName(QStringLiteral("button_box"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(1);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(button_box->sizePolicy().hasHeightForWidth());
        button_box->setSizePolicy(sizePolicy3);
        button_box->setAcceptDrops(false);
        button_box->setOrientation(Qt::Horizontal);
        button_box->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(button_box);


        retranslateUi(io_config_dialog);
        QObject::connect(button_box, SIGNAL(accepted()), io_config_dialog, SLOT(accept()));
        QObject::connect(button_box, SIGNAL(rejected()), io_config_dialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(io_config_dialog);
    } // setupUi

    void retranslateUi(QDialog *io_config_dialog)
    {
        io_config_dialog->setWindowTitle(QApplication::translate("io_config_dialog", "Dialog.InputOverlay.Title", nullptr));
        gb_features->setTitle(QApplication::translate("io_config_dialog", "Dialog.InputOverlay.LocalFeatures", nullptr));
        lbl_local_features->setText(QApplication::translate("io_config_dialog", "Dialog.InputOverlay.LocalFeatures.Label", nullptr));
        cb_iohook->setText(QApplication::translate("io_config_dialog", "Dialog.InputOverlay.EnableUiohook", nullptr));
        cb_gamepad_hook->setText(QApplication::translate("io_config_dialog", "Dialog.InputOverlay.EnableGamepadHook", nullptr));
        cb_enable_overlay->setText(QApplication::translate("io_config_dialog", "Dialog.InputOverlay.EnableInputOverlay", nullptr));
        cb_enable_history->setText(QApplication::translate("io_config_dialog", "Dialog.InputOverlay.EnableInputHistory", nullptr));
        gb_remote->setTitle(QApplication::translate("io_config_dialog", "Dialog.InputOverlay.RemoteConnection", nullptr));
        cb_enable_remote->setText(QApplication::translate("io_config_dialog", "Dialog.InputOverlay.EnableRemoteConnection", nullptr));
        cb_log->setText(QApplication::translate("io_config_dialog", "Dialog.InputOverlay.RemoteConnection.Logging", nullptr));
        lbl_port->setText(QApplication::translate("io_config_dialog", "Dialog.InputOverlay.RemoteConnection.Port", nullptr));
        lbl_status->setText(QApplication::translate("io_config_dialog", "Dialog.InputOverlay.RemoteConnection.Status", nullptr));
        lbl_connections->setText(QApplication::translate("io_config_dialog", "Dialog.InputOverlay.RemoteConnection.Connections", nullptr));
    } // retranslateUi

};

namespace Ui {
    class io_config_dialog: public Ui_io_config_dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGS_DIALOG_H
