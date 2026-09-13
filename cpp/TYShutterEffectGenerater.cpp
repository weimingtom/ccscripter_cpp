//
//  TYShutterEffectGenerater.m
//  Tukuyomi
//
//  Created by toveta on Sat Dec 01 2001.
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

#include "TYShutterEffectGenerater.h"
#include <QPainter>
#include <QDebug>

#define PIXEL_OF_COLUMN 16

extern int VSCREEN_WIDTH;
extern int VSCREEN_HEIGHT;


void TYShutterEffectGenerater::initWithBeforeImage(QImage befImage, QImage atImage, TYEffectDefinition effdef)
{
    TYEffectGenerater::initWithBeforeImage(befImage, atImage, 0);

    switch (effdef.type) {
    case TYEffectTopShutter:
        reverse = true;
        break;
    case TYEffectRightShutter:
        reverse = true;
        // fall through
    case TYEffectLeftShutter:
        horizontal = true;
        break;
    default:
        break;
    }

    phaseInterval = effdef.time / 1000.0 / PIXEL_OF_COLUMN;
    time = effdef.time;
    numberOfColumn = ((horizontal) ? VSCREEN_WIDTH : VSCREEN_HEIGHT) / PIXEL_OF_COLUMN;

    QTimer::singleShot(0, this, SLOT(drawEffect()));

    afterImage = atImage.copy();
    nowImage = befImage.copy();

    startDate = QTime::currentTime();
}

void TYShutterEffectGenerater::drawEffect()
{
    int x = 0, y = 0, w = 0, h = 0;
    int mx = 0, my = 0;
    int phase = 0;
    int movePhase = 0;

    // 経過時間をピクセル単位で取得
    phase = (int)(-startDate.elapsed() / 1000.0 * time / PIXEL_OF_COLUMN * 1000);
    if (phase >= PIXEL_OF_COLUMN) {
        changeEffectionImage(afterImage);
        QTimer::singleShot(0, this, SLOT(effectFinished()));
        return;
    }

    QTimer::singleShot((int)(phaseInterval * 1000), this, SLOT(drawEffect()));

    movePhase = phase -beforePhase;
    // まあ、ありえないとは思うが。
    if(movePhase==0){
        return;
    }

    if(horizontal){
        w = movePhase;
        h = VSCREEN_HEIGHT;
        y = 0;
        my = 0;
        x = (reverse) ? VSCREEN_WIDTH -beforePhase -w : beforePhase;
        mx = (reverse) ? -PIXEL_OF_COLUMN : PIXEL_OF_COLUMN;
    } else {
        w = VSCREEN_WIDTH;
        h = movePhase;
        x = 0;
        mx =0;
        y = (reverse) ? VSCREEN_HEIGHT -beforePhase -h : beforePhase;
        my = (reverse) ? -PIXEL_OF_COLUMN : PIXEL_OF_COLUMN;
    }

    QPainter painter(&nowImage);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    for (int i = 0; i < numberOfColumn; i++, x += mx, y += my) {
        painter.drawImage(x, y, afterImage, x, y, w, h);
    }
    painter.end();

    changeEffectionImage(nowImage);

    beforePhase = phase;
    
    return ;
} // override

TYShutterEffectGenerater::~TYShutterEffectGenerater()
{


}
