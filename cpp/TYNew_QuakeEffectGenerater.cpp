//
//  TYNew_QuakeEffectGenerater.cpp
//  Tukuyomi
//
//  Created by toveta on Sun Jun 23 2002.
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

#include "TYNew_QuakeEffectGenerater.h"
#include <QPainter>
#include <QTimer>

extern int VSCREEN_WIDTH;
extern int VSCREEN_HEIGHT;

TYNew_QuakeEffectGenerater::TYNew_QuakeEffectGenerater(QImage* image, int aInt, int t,
                                                         QObject* parent)
    : TYEffectGenerater(parent)
    , amplitude(0)
    , time(0)
{
    if (image) {
        afterImage = QImage(*image);
    }
    
    nowImage = QImage(VSCREEN_WIDTH, VSCREEN_HEIGHT, QImage::Format_ARGB32);
    
    unionImage = QImage(VSCREEN_WIDTH + aInt * QUEAKE_POWER * 2,
                         VSCREEN_HEIGHT + aInt * QUEAKE_POWER * 2,
                         QImage::Format_ARGB32);
    
    // Draw image on unionImage
    QPainter painter(&unionImage);
    painter.drawImage(aInt * QUEAKE_POWER, aInt * QUEAKE_POWER, *image);
    painter.end();
    
    time = t;
    amplitude = aInt;
    
    QTimer::singleShot(0, this, &TYNew_QuakeEffectGenerater::drawEffect);
    
    startDate = QTime::currentTime();
}

TYNew_QuakeEffectGenerater::~TYNew_QuakeEffectGenerater()
{
}

void TYNew_QuakeEffectGenerater::drawEffect()
{
    if (time + startDate.elapsed() * 1000 < 0) {
        emit changeEffectionImage(afterImage);
        QTimer::singleShot(0, this, &TYEffectGenerater::effectFinished);
        return;
    }
    
    int offsetX = qrand() % (amplitude * QUEAKE_POWER * 3);
    int offsetY = qrand() % (amplitude * QUEAKE_POWER * 3);
    
    QPainter painter(&nowImage);
    painter.drawImage(0, 0, unionImage, offsetX, offsetY, VSCREEN_WIDTH, VSCREEN_HEIGHT);
    painter.end();
    
    emit changeEffectionImage(nowImage);
    
    QTimer::singleShot(0, this, &TYNew_QuakeEffectGenerater::drawEffect);
}
