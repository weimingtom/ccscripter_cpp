//
//  TYMarDLGPlugin.m
//  Tukuyomi
//
//  Created by toveta on Sun Aug 03 2003.
//  Copyright (c) 2003 toveta All rights reserved.
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

#include "TYMarDLGPlugin.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QApplication>

TYMarDLGPlugin::TYMarDLGPlugin(QObject* parent)
    : QObject(parent)
    , FirstName(nullptr)
    , KanaName(nullptr)
    , NickName(nullptr)
    , SecondName(nullptr)
    , dialogWindow(nullptr)
    , loaded(false)
    , resultCode(QDialog::Rejected)
{
}

TYMarDLGPlugin::~TYMarDLGPlugin()
{
    delete dialogWindow;
}

void TYMarDLGPlugin::setupUi()
{
    if (dialogWindow) {
        return;
    }

    dialogWindow = new QWidget();
    dialogWindow->setWindowTitle(tr("Marriage Dialog"));

    QVBoxLayout* mainLayout = new QVBoxLayout(dialogWindow);

    FirstName = new QLineEdit(dialogWindow);
    SecondName = new QLineEdit(dialogWindow);
    KanaName = new QLineEdit(dialogWindow);
    NickName = new QLineEdit(dialogWindow);

    mainLayout->addWidget(new QLabel(tr("First Name:"), dialogWindow));
    mainLayout->addWidget(FirstName);
    mainLayout->addWidget(new QLabel(tr("Second Name:"), dialogWindow));
    mainLayout->addWidget(SecondName);
    mainLayout->addWidget(new QLabel(tr("Kana Name:"), dialogWindow));
    mainLayout->addWidget(KanaName);
    mainLayout->addWidget(new QLabel(tr("Nick Name:"), dialogWindow));
    mainLayout->addWidget(NickName);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* okButton = new QPushButton(tr("OK"), dialogWindow);
    QPushButton* cancelButton = new QPushButton(tr("Cancel"), dialogWindow);

    connect(okButton, &QPushButton::clicked, this, [this]() { dialogOK(nullptr); });
    connect(cancelButton, &QPushButton::clicked, this, [this]() { dialogCancel(nullptr); });

    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);
}

void TYMarDLGPlugin::loadDialog()
{
    if (loaded) {
        return;
    }
    setupUi();
    loaded = true;
}

void TYMarDLGPlugin::execPlugin(QObject* ccsProxy, const QString& aStr)
{
    loadDialog();

    QStringList parts = aStr.split('/');

    FirstName->setText(parts.value(0));
    SecondName->setText(parts.value(1));
    KanaName->setText(parts.value(2));
    NickName->setText(parts.value(3));

    dialogWindow->show();
    QApplication::exec();

    dialogWindow->hide();

    if (resultCode == QDialog::Accepted) {
        QString result = QStringList({FirstName->text(),
                                       SecondName->text(),
                                       KanaName->text(),
                                       NickName->text()}).join('/');
        QMetaObject::invokeMethod(ccsProxy, "setReturnCode",
                                  Q_ARG(int, 1),
                                  Q_ARG(QString, result));
    } else {
        QMetaObject::invokeMethod(ccsProxy, "setReturnCode",
                                  Q_ARG(int, 0),
                                  Q_ARG(QString, aStr));
    }
}

void TYMarDLGPlugin::dialogOK(QObject* sender)
{
    Q_UNUSED(sender);
    resultCode = QDialog::Accepted;
    QApplication::exit();
}

void TYMarDLGPlugin::dialogCancel(QObject* sender)
{
    Q_UNUSED(sender);
    resultCode = QDialog::Rejected;
    QApplication::exit();
}
