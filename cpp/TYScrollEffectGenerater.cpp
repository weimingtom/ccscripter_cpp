//
//  TYScrollEffectGenerater.m
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

#include "TYScrollEffectGenerater.h"
#include <QPainter>
#include <QTimer>

extern int VSCREEN_WIDTH;
extern int VSCREEN_HEIGHT;

TYScrollEffectGenerater::TYScrollEffectGenerater(QImage* befImage, QImage* atImage,
                                                   TYEffectDefinitionValue* effect, QObject* parent)
    : TYEffectGenerater(parent)
    , horizontal(false)
    , reverse(false)
    , phaseInterval(0.0)
    , phaseMAX(0)
    , time(0)
{
    TYEffectDefinition effdef;
	
	
	// 何故か座標が増加する方向にreverse=YESを指定していますが、これはプログラムを間違えてそういう風に作ってしまったからです。(藁
    effect->getValue(&effdef);
    
    switch (effdef.type) {
    case TYEffectBottomScroll:
        reverse = true;
        break;
    case TYEffectLeftScroll:
        reverse = true;
    case TYEffectRightScroll:
        horizontal = true;
        break;
    default:
        break;
    }

    phaseMAX = (horizontal) ? VSCREEN_WIDTH : VSCREEN_HEIGHT ;
    phaseInterval = effdef.time / 1000.0 / phaseMAX;
    time = effdef.time;
    
    QTimer::singleShot(0, this, &TYScrollEffectGenerater::drawEffect);
    
    unionImage = QImage(VSCREEN_WIDTH * (horizontal ? 2 : 1),
                         VSCREEN_HEIGHT * (horizontal ? 1 : 2),
                         QImage::Format_ARGB32);
    nowImage = QImage(VSCREEN_WIDTH, VSCREEN_HEIGHT, QImage::Format_ARGB32);
    
    QPainter painter(&unionImage);
    int x = VSCREEN_WIDTH * (reverse ? 1 : 0) * (horizontal ? 1 : 0);
    int y = VSCREEN_HEIGHT * (reverse ? 1 : 0) * (horizontal ? 0 : 1);
    if (befImage) {
        painter.drawImage(x, y, *befImage);
    }
    x = VSCREEN_WIDTH * (reverse ? 0 : 1) * (horizontal ? 1 : 0);
    y = VSCREEN_HEIGHT * (reverse ? 0 : 1) * (horizontal ? 0 : 1);
    if (atImage) {
        painter.drawImage(x, y, *atImage);
    }
    painter.end();
    
    startDate = QTime::currentTime();
}

TYScrollEffectGenerater::~TYScrollEffectGenerater()
{
}

void TYScrollEffectGenerater::drawEffect()
{
    int x,y;
    int phase;
    
    // 経過時間をイメージの高さ幅のピクセル単位で取得
    phase = (int)(-startDate.elapsed() * 1000 / time * phaseMAX);
    if (phase >= phaseMAX) {
        // エフェクト終了
        x = VSCREEN_WIDTH * (reverse ? 0 : 1) * (horizontal ? 1 : 0);
        y = VSCREEN_HEIGHT * (reverse ? 0 : 1) * (horizontal ? 0 : 1);
        
        QPainter painter(&nowImage);
        painter.drawImage(0, 0, unionImage, x, y, VSCREEN_WIDTH, VSCREEN_HEIGHT);
        painter.end();
        
        emit changeEffectionImage(nowImage);
        QTimer::singleShot(0, this, &TYEffectGenerater::effectFinished);
    } else {
        QTimer::singleShot((int)(phaseInterval * 1000), this, &TYScrollEffectGenerater::drawEffect);
        x = (VSCREEN_WIDTH * (reverse ? 1 : 0) + phase * (reverse ? -1 : 1)) * (horizontal ? 1 : 0);
        y = (VSCREEN_HEIGHT * (reverse ? 1 : 0) + phase * (reverse ? -1 : 1)) * (horizontal ? 0 : 1);
        
        QPainter painter(&nowImage);
        painter.drawImage(0, 0, unionImage, x, y, VSCREEN_WIDTH, VSCREEN_HEIGHT);
        painter.end();
        
        emit changeEffectionImage(nowImage);
    }
}
