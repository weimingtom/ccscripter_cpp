//
//  TYPreferencePanelController.m
//  Tukuyomi
//
//  Created by toveta on Sat Jul 20 2002.
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

#include "TYPreferencePanelController.h"
#include "TYEnvironment.h"
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QApplication>

TYPreferencePanelController* TYPreferencePanelController::sharedObject = nullptr;

int TYPreferencePanelController::showDialog()
{
    if(!sharedObject)
        sharedObject = new TYPreferencePanelController();

    sharedObject->loadEnvironment();
    sharedObject->panel->show();
    sharedObject->panel->raise();
    sharedObject->panel->activateWindow();
    
    return sharedObject->exec();
}

TYPreferencePanelController::TYPreferencePanelController(QWidget *parent)
    : QDialog(parent)
    , bufferPage(nullptr)
    , bufferStepper(nullptr)
    , enableAutoReplay(nullptr)
    , enableBold(nullptr)
    , enableMovie(nullptr)
    , hideOtherFullScreen(nullptr)
    , panel(this)
    , scaleInFullScreen(nullptr)
{
    setWindowTitle(tr("Preferences"));
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    QHBoxLayout *bufferLayout = new QHBoxLayout();
    bufferLayout->addWidget(new QLabel(tr("Lookback Buffer Pages:"), this));
    bufferPage = new QSpinBox(this);
    bufferStepper = new QSpinBox(this);
    bufferLayout->addWidget(bufferPage);
    bufferLayout->addWidget(bufferStepper);
    mainLayout->addLayout(bufferLayout);
    
    scaleInFullScreen = new QCheckBox(tr("Scale in Full Screen"), this);
    enableBold = new QCheckBox(tr("Enable Bold Font"), this);
    enableAutoReplay = new QCheckBox(tr("Enable Auto Replay"), this);
    enableMovie = new QCheckBox(tr("Enable Movie"), this);
    hideOtherFullScreen = new QCheckBox(tr("Hide Other in Full Screen"), this);
    
    mainLayout->addWidget(scaleInFullScreen);
    mainLayout->addWidget(enableBold);
    mainLayout->addWidget(enableAutoReplay);
    mainLayout->addWidget(enableMovie);
    mainLayout->addWidget(hideOtherFullScreen);
    
    connect(scaleInFullScreen, &QCheckBox::stateChanged, this, &TYPreferencePanelController::changeFullScreen);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton(tr("OK"), this);
    QPushButton *cancelButton = new QPushButton(tr("Cancel"), this);
    
    connect(okButton, &QPushButton::clicked, this, &TYPreferencePanelController::dialogOK);
    connect(cancelButton, &QPushButton::clicked, this, &TYPreferencePanelController::dialogCancel);
    
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);
}

TYPreferencePanelController::~TYPreferencePanelController()
{
}

void TYPreferencePanelController::loadEnvironment()
{
    scaleInFullScreen->setChecked(TYEnvironment::boolForKey(TYDisplayResizeAtFullScreenEnvironment));
    enableBold->setChecked(TYEnvironment::boolForKey(TYEnableBoldFontEnvironment));
    enableAutoReplay->setChecked(TYEnvironment::boolForKey(TYLookbackVoiceReplayEnvironment));
    
    int bufferValue = TYEnvironment::objectForKey(TYLookbackBufferPageEnvironment).toInt();
    bufferStepper->setValue(bufferValue);
    bufferPage->setValue(bufferValue);
    
    enableMovie->setChecked(TYEnvironment::boolForKey(TYEnablePlayMovieEnvironment));
    hideOtherFullScreen->setChecked(TYEnvironment::boolForKey(TYHideOtherFullScreenEnvironment));
    hideOtherFullScreen->setEnabled(scaleInFullScreen->isChecked());
}

QDialog* TYPreferencePanelController::panel()
{
    return panel;
}

void TYPreferencePanelController::dialogOK()
{
    hide();
    
    TYEnvironment::setObject(QVariant(scaleInFullScreen->isChecked()), 
                             TYDisplayResizeAtFullScreenEnvironment);
    TYEnvironment::setObject(QVariant(enableBold->isChecked()), 
                             TYEnableBoldFontEnvironment);
    TYEnvironment::setObject(QVariant(enableAutoReplay->isChecked() ? 1 : 0), 
                             TYLookbackVoiceReplayEnvironment);
    TYEnvironment::setObject(QVariant(bufferPage->value()), 
                             TYLookbackBufferPageEnvironment);
    TYEnvironment::setObject(QVariant(enableMovie->isChecked()), 
                             TYEnablePlayMovieEnvironment);
    TYEnvironment::setObject(QVariant(hideOtherFullScreen->isChecked()), 
                             TYHideOtherFullScreenEnvironment);
    
    done(QDialog::Accepted);
}

void TYPreferencePanelController::dialogCancel()
{
    hide();
    done(QDialog::Rejected);
}

void TYPreferencePanelController::changeFullScreen()
{
    hideOtherFullScreen->setEnabled(scaleInFullScreen->isChecked());
}
