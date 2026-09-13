//
//  TYMosaicEffectGenerater.h
//  Tukuyomi
//
//  Created by toveta on Wed Nov 06 2002.
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

#ifndef TYMOSAICEFFECTGENERATER_H
#define TYMOSAICEFFECTGENERATER_H

#include "TYEffectGenerater.h"
#include <QImage>
#include <QTimer>

#define PHASE_NUM 6

class TYMosaicEffectGenerater : public TYEffectGenerater {
    Q_OBJECT
public:
    explicit TYMosaicEffectGenerater(QObject* parent = nullptr);
    TYMosaicEffectGenerater(QImage* befImage, QImage* atImage, TYEffectDefinitionValue* effect,
                           QObject* parent = nullptr);
    virtual ~TYMosaicEffectGenerater();

public slots:
    void drawEffect() override;

private:
    bool m_inToward;
    unsigned char* m_nowBitmapData;
    QImage* m_sourceBitmap;
    double m_phaseInterval;
    int m_phase;
};

// private method
#endif // TYMOSAICEFFECTGENERATER_H
