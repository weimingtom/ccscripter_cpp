//
//  TYAutoModeController.m
//  Tukuyomi
//
//  Created by toveta on Mon May 27 2002.
//  Copyright (c) 2002 toveta All rights reserved.
//
/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY 
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 * &ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "TYAutoModeController.h"
#include <QApplication>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QLabel>

TYAutoModeController::TYAutoModeController()
    : QObject(nullptr)
    , TextSpeed(nullptr)
    , WaitNum(nullptr)
    , WaitType(nullptr)
    , m_dialog(nullptr)
{
    m_dialog = new QDialog();
    m_dialog->setWindowTitle(QString::fromLocal8Bit("Auto Mode Settings"));

    TextSpeed = new QSpinBox();
    TextSpeed->setRange(1, 100);
    WaitNum = new QSpinBox();
    WaitNum->setRange(0, 9999);

    WaitType = new QButtonGroup(m_dialog);
    QRadioButton* radio1 = new QRadioButton(QString::fromLocal8Bit("Wait by character count"));
    QRadioButton* radio2 = new QRadioButton(QString::fromLocal8Bit("Wait until display ends"));
    WaitType->addButton(radio1, TYNumOfCharsAutomodeWait);
    WaitType->addButton(radio2, TYEndFromDispAutomodeWait);

    QDialogButtonBox* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel
    );

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(new QLabel(QString::fromLocal8Bit("Text Speed:")));
    layout->addWidget(TextSpeed);
    layout->addWidget(new QLabel(QString::fromLocal8Bit("Wait Value:")));
    layout->addWidget(WaitNum);
    layout->addWidget(radio1);
    layout->addWidget(radio2);
    layout->addWidget(buttons);

    m_dialog->setLayout(layout);

    connect(buttons, &QDialogButtonBox::accepted, this, &TYAutoModeController::dialogOk);
    connect(buttons, &QDialogButtonBox::rejected, this, &TYAutoModeController::dialogCancel);
}

TYAutoModeController::~TYAutoModeController()
{
    if (m_dialog) {
        delete m_dialog;
    }
}

TYAutoModeController* TYAutoModeController::dialog()
{
    return new TYAutoModeController();
}

int TYAutoModeController::runModalTextSpeed(int speed, int wait, TYAutoModeWaitType type)
{
    TextSpeed->setValue(speed);
    WaitNum->setValue(wait);

    QAbstractButton* button = WaitType->button(type);
    if (button) {
        button->setChecked(true);
    }

    m_dialog->show();
    m_dialog->raise();
    m_dialog->activateWindow();

    int result = m_dialog->exec();
    return (result == QDialog::Accepted) ? 1 : 0;
}

int TYAutoModeController::textSpeed() { return TextSpeed->value(); }
int TYAutoModeController::waitType() { int id = WaitType->checkedId(); return (id == 0) ? TYNumOfCharsAutomodeWait : TYEndFromDispAutomodeWait; }
int TYAutoModeController::waitNum() { return WaitNum->value(); }

void TYAutoModeController::dialogOk()
{
    m_dialog->done(QDialog::Accepted);
}

void TYAutoModeController::dialogCancel()
{
    m_dialog->done(QDialog::Rejected);
}
