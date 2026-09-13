//
//  TYVerticalScrollGenerater.m
//  Tukuyomi
//
//  Created by toveta on Fri Nov 30 2001.
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


#include "TYVerticalScrollGenerater.h"
#include <QPainter>
#include <QDebug>

extern int VSCREEN_WIDTH;
extern int VSCREEN_HEIGHT;
void TYVerticalScrollGenerater::initWithBeforeImage(QImage befImage, QImage atImage, TYEffectDefinition effdef, bool aBool)
{
    TYEffectGenerater::initWithBeforeImage(befImage, atImage, 0);

    reverse = aBool;
    
    phaseInterval = effdef.time / 1000.0 / VSCREEN_HEIGHT;
    time = effdef.time;

    QTimer::singleShot(0, this, SLOT(drawEffect()));

    unionImage = QImage(VSCREEN_WIDTH, VSCREEN_HEIGHT * 2, QImage::Format_ARGB32);
    nowImage = QImage(VSCREEN_WIDTH, VSCREEN_HEIGHT, QImage::Format_ARGB32);

    QPainter unionPainter(&unionImage);
    unionPainter.setCompositionMode(QPainter::CompositionMode_Source);
    unionPainter.drawImage(QPoint(0, VSCREEN_HEIGHT * (reverse ? 1 : 0)), befImage);
    unionPainter.drawImage(QPoint(0, VSCREEN_HEIGHT * (reverse ? 0 : 1)), atImage);
    unionPainter.end();

    startDate = QTime::currentTime();
}

// private method
void TYVerticalScrollGenerater::drawEffect()
{
    int phase = 0;
    // 経過時間を480分割単位で取得
    phase = (int)(-startDate.elapsed() / 1000.0 * time / VSCREEN_HEIGHT * 1000);
    if(phase >= VSCREEN_HEIGHT){
        // エフェクト終了
        QPainter painter(&nowImage);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.drawImage(QPoint(0, 0), unionImage,
                         0, (VSCREEN_HEIGHT * ((reverse) ? -1 : 1)) + VSCREEN_HEIGHT * (reverse ? 1 : 0),
                         VSCREEN_WIDTH, VSCREEN_HEIGHT);
        painter.end();
        changeEffectionImage(nowImage);
        QTimer::singleShot(0, this, SLOT(effectFinished()));
    } else {
        QTimer::singleShot((int)(phaseInterval * 1000), this, SLOT(drawEffect()));

        QPainter painter(&nowImage);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.drawImage(QPoint(0, 0), unionImage,
                         0, (phase * ((reverse) ? -1 : 1)) + VSCREEN_HEIGHT * (reverse ? 1 : 0),
                         VSCREEN_WIDTH, VSCREEN_HEIGHT);
        painter.end();

        changeEffectionImage(nowImage);
    }

    return;
} // override

TYVerticalScrollGenerater::~TYVerticalScrollGenerater()
{




}


