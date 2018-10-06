/********************************************************************************
** Form generated from reading UI file 'settingswFmyGB.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef SETTINGSWFMYGB_H
#define SETTINGSWFMYGB_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_inptoverlayconfig
{
public:
    QDialogButtonBox *button_box;
    QGroupBox *gb_startup;
    QCheckBox *cb_iohook;
    QCheckBox *cb_gamepad_hook;
    QCheckBox *cb_enable_overlay;
    QCheckBox *cb_enable_history;
    QLabel *lbl_local_features;
    QGroupBox *gb_remoteconnection;
    QCheckBox *cb_enable_remote;
    QLabel *lbl_status;
    QSpinBox *box_port;
    QLabel *lbl_port;
    QCheckBox *cb_log;
    QListWidget *box_connections;
    QLabel *lbl_connections;

    void setupUi(QDialog *inptoverlayconfig)
    {
        if (inptoverlayconfig->objectName().isEmpty())
            inptoverlayconfig->setObjectName(QStringLiteral("inptoverlayconfig"));
        inptoverlayconfig->resize(300, 450);
        inptoverlayconfig->setMinimumSize(QSize(300, 450));
        inptoverlayconfig->setMaximumSize(QSize(300, 450));
        inptoverlayconfig->setSizeIncrement(QSize(300, 450));
        button_box = new QDialogButtonBox(inptoverlayconfig);
        button_box->setObjectName(QStringLiteral("button_box"));
        button_box->setGeometry(QRect(140, 420, 156, 23));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(button_box->sizePolicy().hasHeightForWidth());
        button_box->setSizePolicy(sizePolicy);
        button_box->setAcceptDrops(false);
        button_box->setOrientation(Qt::Horizontal);
        button_box->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        gb_startup = new QGroupBox(inptoverlayconfig);
        gb_startup->setObjectName(QStringLiteral("gb_startup"));
        gb_startup->setGeometry(QRect(10, 10, 281, 131));
        cb_iohook = new QCheckBox(gb_startup);
        cb_iohook->setObjectName(QStringLiteral("cb_iohook"));
        cb_iohook->setGeometry(QRect(10, 40, 194, 17));
        cb_gamepad_hook = new QCheckBox(gb_startup);
        cb_gamepad_hook->setObjectName(QStringLiteral("cb_gamepad_hook"));
        cb_gamepad_hook->setGeometry(QRect(10, 60, 225, 17));
        cb_enable_overlay = new QCheckBox(gb_startup);
        cb_enable_overlay->setObjectName(QStringLiteral("cb_enable_overlay"));
        cb_enable_overlay->setGeometry(QRect(10, 80, 220, 17));
        cb_enable_history = new QCheckBox(gb_startup);
        cb_enable_history->setObjectName(QStringLiteral("cb_enable_history"));
        cb_enable_history->setGeometry(QRect(10, 100, 216, 17));
        lbl_local_features = new QLabel(gb_startup);
        lbl_local_features->setObjectName(QStringLiteral("lbl_local_features"));
        lbl_local_features->setGeometry(QRect(10, 20, 197, 13));
        gb_remoteconnection = new QGroupBox(inptoverlayconfig);
        gb_remoteconnection->setObjectName(QStringLiteral("gb_remoteconnection"));
        gb_remoteconnection->setGeometry(QRect(10, 150, 281, 261));
        cb_enable_remote = new QCheckBox(gb_remoteconnection);
        cb_enable_remote->setObjectName(QStringLiteral("cb_enable_remote"));
        cb_enable_remote->setGeometry(QRect(10, 20, 247, 17));
        lbl_status = new QLabel(gb_remoteconnection);
        lbl_status->setObjectName(QStringLiteral("lbl_status"));
        lbl_status->setGeometry(QRect(10, 240, 227, 13));
        box_port = new QSpinBox(gb_remoteconnection);
        box_port->setObjectName(QStringLiteral("box_port"));
        box_port->setGeometry(QRect(10, 80, 241, 22));
        box_port->setButtonSymbols(QAbstractSpinBox::NoButtons);
        box_port->setProperty("showGroupSeparator", QVariant(false));
        box_port->setMaximum(65535);
        box_port->setValue(1608);
        lbl_port = new QLabel(gb_remoteconnection);
        lbl_port->setObjectName(QStringLiteral("lbl_port"));
        lbl_port->setGeometry(QRect(10, 60, 216, 13));
        cb_log = new QCheckBox(gb_remoteconnection);
        cb_log->setObjectName(QStringLiteral("cb_log"));
        cb_log->setGeometry(QRect(10, 40, 256, 17));
        box_connections = new QListWidget(gb_remoteconnection);
        box_connections->setObjectName(QStringLiteral("box_connections"));
        box_connections->setGeometry(QRect(10, 130, 256, 101));
        lbl_connections = new QLabel(gb_remoteconnection);
        lbl_connections->setObjectName(QStringLiteral("lbl_connections"));
        lbl_connections->setGeometry(QRect(10, 110, 285, 13));

        retranslateUi(inptoverlayconfig);
        QObject::connect(button_box, SIGNAL(accepted()), inptoverlayconfig, SLOT(accept()));
        QObject::connect(button_box, SIGNAL(rejected()), inptoverlayconfig, SLOT(reject()));

        QMetaObject::connectSlotsByName(inptoverlayconfig);
    } // setupUi

    void retranslateUi(QDialog *inptoverlayconfig)
    {
        inptoverlayconfig->setWindowTitle(QApplication::translate("inptoverlayconfig", "Dialog.InputOverlay.Title", nullptr));
        gb_startup->setTitle(QApplication::translate("inptoverlayconfig", "Dialog.InputOverlay.LocalFeatures", nullptr));
        cb_iohook->setText(QApplication::translate("inptoverlayconfig", "Dialog.InputOverlay.EnableUiohook", nullptr));
        cb_gamepad_hook->setText(QApplication::translate("inptoverlayconfig", "Dialog.InputOverlay.EnableGamepadHook", nullptr));
        cb_enable_overlay->setText(QApplication::translate("inptoverlayconfig", "Dialog.InputOverlay.EnableInputOverlay", nullptr));
        cb_enable_history->setText(QApplication::translate("inptoverlayconfig", "Dialog.InputOverlay.EnableInputHistory", nullptr));
        lbl_local_features->setText(QApplication::translate("inptoverlayconfig", "Dialog.InputOverlay.LocalFeatures.Label", nullptr));
        gb_remoteconnection->setTitle(QApplication::translate("inptoverlayconfig", "Dialog.InputOverlay.RemoteConnection", nullptr));
        cb_enable_remote->setText(QApplication::translate("inptoverlayconfig", "Dialog.InputOverlay.EnableRemoteConnection", nullptr));
        lbl_status->setText(QApplication::translate("inptoverlayconfig", "Dialog.InputOverlay.RemoteConnection.Status", nullptr));
        lbl_port->setText(QApplication::translate("inptoverlayconfig", "Dialog.InputOverlay.RemoteConnection.Port", nullptr));
        cb_log->setText(QApplication::translate("inptoverlayconfig", "Dialog.InputOverlay.RemoteConnection.Logging", nullptr));
        lbl_connections->setText(QApplication::translate("inptoverlayconfig", "Dialog.InputOverlay.RemoteConnection.Connections", nullptr));
    } // retranslateUi

};

namespace Ui {
    class inptoverlayconfig: public Ui_inptoverlayconfig {};
} // namespace Ui

QT_END_NAMESPACE

#endif // SETTINGSWFMYGB_H
