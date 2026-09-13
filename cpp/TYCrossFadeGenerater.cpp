//
//  TYCrossFadeGenerater.m
//  Tukuyomi
//
//  Created by toveta on Wed Nov 21 2001.
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

#include "TYCrossFadeGenerater.h"
#include "TYStageManager.h"
#include "TYImageUtil.h"

TYCrossFadeGenerater::TYCrossFadeGenerater(QObject *parent)
    : TYEffectGenerater(parent)
    , m_nowBitmapptr(nullptr)
    , m_phaseInterval(0)
    , m_time(0)
{
}

TYCrossFadeGenerater::~TYCrossFadeGenerater() {
    if (m_nowBitmapptr) {
        free(m_nowBitmapptr);
    }
}

void TYCrossFadeGenerater::initWithBeforeImage(const QImage& befImage, const QImage& atImage, TYEffectDefinitionValue* effect) {
    TYEffectDefinition effdef;
    effect->getValue(&effdef);
    
    m_phaseInterval = effdef.time / 1000.0 / 256.0;
    m_time = effdef.time;
    
    QTimer::singleShot(0, this, SLOT(drawEffect()));
    
    m_afterImage = atImage;
    /*
    if(afterImage)
        [ afterImage retain ];
    */
    
    m_beforeBitmap = befImage;
    if (m_beforeBitmap.isNull()) {
        m_beforeBitmap = blankBitmap(VSCREEN_WIDTH, VSCREEN_HEIGHT);
    }
    
    m_afterBitmap = atImage;
    if (m_afterBitmap.isNull()) {
        m_afterBitmap = blankBitmap(VSCREEN_WIDTH, VSCREEN_HEIGHT);
    }
    
    m_nowImage = QImage(VSCREEN_WIDTH, VSCREEN_HEIGHT, QImage::Format_RGB32);
    
    int bufferSize = VSCREEN_WIDTH * VSCREEN_HEIGHT * 3;
    m_nowBitmapptr = static_cast<unsigned char*>(malloc(bufferSize));
    
    m_startDate = QDateTime::currentDateTime();
}

// private method
void TYCrossFadeGenerater::drawEffect() {
    int pixel = VSCREEN_WIDTH * VSCREEN_HEIGHT;
    

    // 経過時間を256分割単位で取得
    int phase = (-m_startDate.msecsTo(QDateTime::currentDateTime())) / m_time * 256;
    
    if (phase >= 256) {
        // エフェクト終了
        emit changeEffectionImage(m_afterImage);
        QTimer::singleShot(0, this, SLOT(effectFinished()));
        return;
    }
    
    QTimer::singleShot(static_cast<int>(m_phaseInterval * 1000), this, SLOT(drawEffect()));
    
    const unsigned char* src = m_afterBitmap.bits();
    const unsigned char* dst = m_beforeBitmap.bits();
    unsigned char* now = m_nowBitmapptr;
    
    int spps = 3;
    int sppd = 3;
    int sppn = 3;
    
    for (int i = 0; i < pixel; i++, src += spps, dst += sppd, now += sppn) {
        now[0] = (src[0] * phase + dst[0] * (0xFF - phase)) >> 8;
        now[1] = (src[1] * phase + dst[1] * (0xFF - phase)) >> 8;
        now[2] = (src[2] * phase + dst[2] * (0xFF - phase)) >> 8;
    }
    
    // Convert raw bitmap data to QImage
    m_nowImage = QImage(m_nowBitmapptr, VSCREEN_WIDTH, VSCREEN_HEIGHT, VSCREEN_WIDTH * 3, QImage::Format_RGB888).copy();
    
    emit changeEffectionImage(m_nowImage);
}
