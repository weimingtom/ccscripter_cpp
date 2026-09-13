//
//  TYPatternCrossFadeGenerater.m
//  Tukuyomi
//
//  Created by toveta on Mon Nov 04 2002.
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

#include "TYPatternCrossFadeGenerater.h"
#include "TYEffectPatternMap.h"
#include "TYResourceServer.h"

extern int VSCREEN_WIDTH;
extern int VSCREEN_HEIGHT;

TYPatternCrossFadeGenerater::TYPatternCrossFadeGenerater(QObject* parent)
    : TYEffectGenerater(parent)
    , m_patternPtr(nullptr)
    , m_patternWide(0)
    , m_patternHigh(0)
    , m_beforeBitmap(nullptr)
    , m_afterBitmap(nullptr)
    , m_nowBitmapData(nullptr)
    , m_phaseInterval(0)
    , m_time(0)
{
}

TYPatternCrossFadeGenerater::TYPatternCrossFadeGenerater(QImage* befImage, QImage* atImage,
                                                         TYEffectDefinitionValue* effect, QObject* parent)
    : TYEffectGenerater(parent)
    , m_patternPtr(nullptr)
    , m_beforeBitmap(nullptr)
    , m_afterBitmap(nullptr)
    , m_nowBitmapData(nullptr)
    , m_phaseInterval(0)
    , m_time(0)
{
    TYEffectDefinition effdef;
    effect->getValue(&effdef);
    TYEffectPatternMap* patternMap = TYResourceServer::sharedServer()->getEffectPattern(effdef.path);
    if (!patternMap) {
        qWarning("Can't load pattern Map");
        return;
    }

    m_patternPtr = patternMap->patternMapData();
    m_patternWide = patternMap->pixelsWide();
    m_patternHigh = patternMap->pixelsHigh();

    m_phaseInterval = effdef.time / 1000.0 / 256.0;
    m_time = effdef.time;

    QTimer::singleShot(0, this, &TYPatternCrossFadeGenerater::drawEffect);

    m_afterImage = atImage ? new QImage(*atImage) : nullptr;
    /*
    if(afterImage)
        [ afterImage retain ];
    */
    m_beforeBitmap = befImage ? new QImage(*befImage) : nullptr;
    m_afterBitmap = atImage ? new QImage(*atImage) : nullptr;

    m_nowImage = new QImage(VSCREEN_WIDTH, VSCREEN_HEIGHT, QImage::Format_RGB888);
    m_nowBitmapData = new unsigned char[VSCREEN_WIDTH * VSCREEN_HEIGHT * 3];

    m_startDate = QDateTime::currentDateTime();
}

TYPatternCrossFadeGenerater::~TYPatternCrossFadeGenerater()
{
    if (m_beforeBitmap) delete m_beforeBitmap;
    if (m_afterBitmap) delete m_afterBitmap;
    if (m_nowBitmapData) delete[] m_nowBitmapData;
}

// private method
void TYPatternCrossFadeGenerater::drawEffect()
{
    unsigned char *src, *dst, *now;
    int phase;
    // 経過時間を256分割単位で取得
    double elapsed = m_startDate.msecsTo(QDateTime::currentDateTime()) / 1000.0;
    phase = static_cast<int>(elapsed / m_time * 256);

    if (phase >= 256) {
        // エフェクト終了
        emit changeEffectionImage(m_afterImage);
        QTimer::singleShot(0, this, &TYEffectGenerater::effectFinished);
        return;
    }

    QTimer::singleShot(static_cast<int>(m_phaseInterval * 1000), this, &TYPatternCrossFadeGenerater::drawEffect);

    if (!m_beforeBitmap || !m_afterBitmap || !m_patternPtr)
        return;

    dst = m_beforeBitmap->bits();
    src = m_afterBitmap->bits();
    now = m_nowBitmapData;

    for (int h = 0, ph = 0; h < VSCREEN_HEIGHT; h++) {
        for (int w = 0, pw = 0; w < VSCREEN_WIDTH; w++) {
            int patternIdx = m_patternWide * ph + pw;
            int ptval = -m_patternPtr[patternIdx] + (phase << 1);
            if (ptval < 0)
                ptval = 0;
            else if (ptval > 0xFF)
                ptval = 0xFF;
            now[0] = (src[0] * ptval + dst[0] * (0xFF - ptval)) >> 8;
            now[1] = (src[1] * ptval + dst[1] * (0xFF - ptval)) >> 8;
            now[2] = (src[2] * ptval + dst[2] * (0xFF - ptval)) >> 8;
            pw++;
            if (pw >= m_patternWide)
                pw = 0;

            src += 3;
            dst += 3;
            now += 3;
        }
        ph++;
        if (ph >= m_patternHigh)
            ph = 0;
    }

    memcpy(m_nowImage->bits(), m_nowBitmapData, VSCREEN_WIDTH * VSCREEN_HEIGHT * 3);

    emit changeEffectionImage(m_nowImage);
}
