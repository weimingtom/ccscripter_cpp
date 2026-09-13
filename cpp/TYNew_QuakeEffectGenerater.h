//
//  TYNew_QuakeEffectGenerater.h
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

#ifndef TYNew_QuakeEffectGenerater_h
#define TYNew_QuakeEffectGenerater_h

#include "TYEffectGenerater.h"
#include <QObject>
#include <QImage>
#include <QTime>

// 振幅の係数
#define QUEAKE_POWER 8

// quake命令のエフェクト
class TYNew_QuakeEffectGenerater : public TYEffectGenerater {
    Q_OBJECT
public:
    TYNew_QuakeEffectGenerater(QImage* aImage, int aInt, int t,
                                 QObject* parent = nullptr);
    virtual ~TYNew_QuakeEffectGenerater();

protected slots:
    void drawEffect();

private:
    QImage unionImage;
    int amplitude;
    int time;
    QTime startDate;
};

#endif // TYNew_QuakeEffectGenerater_h
