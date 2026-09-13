//
//  TYAudioPathController.m
//  Tukuyomi
//
//  Created by toveta on Sat Jan 19 2002.
//  Copyright (c) 2001 toveta All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY toveta ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL toveta OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * &ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "TYAudioPathController.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>

TYAudioPathController::TYAudioPathController(QObject* parent)
    : QObject(parent)
    , window(nullptr)
    , directory(nullptr)
    , filename(nullptr)
    , delegate(nullptr)
    , errorField(nullptr)
{
}

TYAudioPathController::~TYAudioPathController()
{
}

void TYAudioPathController::dialogOk()
{
    QString fn;
    QString path;

    fn = filename->text();
    if (fn.isEmpty()) {
        QApplication::beep();
        errorField->setText(QString::fromLocal8Bit("Please be sure to input a file name."));
        return;
    }

    if (!fn.contains('*')) {
        QApplication::beep();
        errorField->setText(QString::fromLocal8Bit("An asterisk is required for a file name."));
        return;
    }

    path = directory->text() + "/" + fn;

    changeAudioPath(path);
    window->hide();
}

void TYAudioPathController::dialogCancel()
{
    window->hide();
}

void TYAudioPathController::restoreDefault()
{
    QString dir;
    QString file;

    int lastSlash = defaultPath.lastIndexOf('/');
    dir = defaultPath.left(lastSlash);
    file = defaultPath.mid(lastSlash + 1);

    directory->setText(dir);
    filename->setText(file);
}

void TYAudioPathController::selectDirectory()
{
    QString selectedDir = QFileDialog::getExistingDirectory(
        window,
        QString::fromLocal8Bit("Select Directory"),
        directory->text()
    );

    if (!selectedDir.isEmpty()) {
        directory->setText(selectedDir);
    }
}

void TYAudioPathController::openWithPath(const QString& aPath, const QString& defPath)
{
    defaultPath = defPath;
    initPath = aPath;

    int lastSlash = initPath.lastIndexOf('/');
    QString dir = initPath.left(lastSlash);
    QString file = initPath.mid(lastSlash + 1);

    directory->setText(dir);
    filename->setText(file);

    window->show();
    window->raise();
}

void TYAudioPathController::setDelegate(QObject* aObject)
{
    delegate = aObject;
}

    // delegate implementation
void TYAudioPathController::changeAudioPath(const QString& aPath)
{
    if(delegate)
        QMetaObject::invokeMethod(delegate, "changeAudioPath", Q_ARG(QString, aPath));
}
