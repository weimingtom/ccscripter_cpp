//
//  TYQuakeEffectGenerater.m
//  Tukuyomi
//
//  Created by toveta on Tue Dec 11 2001.
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

#include "TYQuakeEffectGenerater.h"
#include <QPainter>
#include <QTimer>

#define SHAKE_SIZE 12

extern int VSCREEN_WIDTH;
extern int VSCREEN_HEIGHT;

TYQuakeEffectGenerater::TYQuakeEffectGenerater(QImage* image, int type, int times, int second,
                                                 QObject* parent)
    : TYEffectGenerater(parent)
    , quakeType(TYQuakeTypeHorizontal)
    , quakeTimes(0)
    , quakeSecond(0)
    , horizontal(false)
    , timeOfShake(0)
    , timeOfQuarter(0)
{
    if (image) {
        afterImage = QImage(*image);
    }
    
    quakeType = (TYQuakeType)type;
    if (quakeType == TYQuakeTypeHorizontal) {
        horizontal = true;
    }
    quakeTimes = times;
    quakeSecond = second;
    timeOfShake = quakeSecond / quakeTimes;
    timeOfQuarter = timeOfShake >> 2;
    
    unionImage = QImage(VSCREEN_WIDTH + SHAKE_SIZE * times * (horizontal ? 1 : 0) * 2,
                         VSCREEN_HEIGHT + SHAKE_SIZE * times * (horizontal ? 0 : 1) * 2,
                         QImage::Format_ARGB32);
    nowImage = QImage(VSCREEN_WIDTH, VSCREEN_HEIGHT, QImage::Format_ARGB32);
    
    unionImage.fill(Qt::black);
    
    QPainter painter(&unionImage);
    painter.drawImage(SHAKE_SIZE * times * (horizontal ? 1 : 0),
                     SHAKE_SIZE * times * (horizontal ? 0 : 1),
                     *image);
    painter.end();
    
    QTimer::singleShot(0, this, &TYQuakeEffectGenerater::drawEffect);
    
    startDate = QTime::currentTime();
}

TYQuakeEffectGenerater::~TYQuakeEffectGenerater()
{
}

void TYQuakeEffectGenerater::drawEffect()
{
    int restTime;
    int shakeLevel;
    int shakeQuarter;
    int x, y;
    int phaseOfQuarter;
    
    restTime = quakeSecond + startDate.elapsed() * 1000;
    shakeLevel = restTime / timeOfShake + 1;
    
    if (restTime <= 0) {
        emit changeEffectionImage(afterImage);
        QTimer::singleShot(0, this, &TYEffectGenerater::effectFinished);
        return;
    }
    
    if (horizontal) {
        x = 0;
        y = 0;
    } else {
        x = 0;
        y = 0;
    }
    
    *(&x + (horizontal ? 0 : 1)) = SHAKE_SIZE * quakeTimes;
    
    phaseOfQuarter = restTime % timeOfQuarter;
    shakeQuarter = (restTime / timeOfQuarter) % 4;
    // １回分の揺れ時間をさらに4分割して、どの時点か求める。
    switch(shakeQuarter){
    case 1:
        *(&x + (horizontal ? 0 : 1)) += SHAKE_SIZE * shakeLevel * phaseOfQuarter / timeOfQuarter;
        break;
    case 2:
        *(&x + (horizontal ? 0 : 1)) += SHAKE_SIZE * shakeLevel * (timeOfQuarter - phaseOfQuarter) / timeOfQuarter;
        break;
    case 3:
        *(&x + (horizontal ? 0 : 1)) -= SHAKE_SIZE * shakeLevel * phaseOfQuarter / timeOfQuarter;
        break;
    case 0:
        *(&x + (horizontal ? 0 : 1)) -= SHAKE_SIZE * shakeLevel * (timeOfQuarter - phaseOfQuarter) / timeOfQuarter;
        break;
    }
    
    QPainter painter(&nowImage);
    painter.drawImage(0, 0, unionImage, x, y, VSCREEN_WIDTH, VSCREEN_HEIGHT);
    painter.end();
    
    emit changeEffectionImage(nowImage);
    
    QTimer::singleShot(0, this, &TYQuakeEffectGenerater::drawEffect);
}

void TYQuakeEffectGenerater::quakeFinished()
{
    emit effectFinished();
}
