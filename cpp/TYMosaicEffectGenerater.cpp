//
//  TYMosaicEffectGenerater.m
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

#include "TYMosaicEffectGenerater.h"

extern int VSCREEN_WIDTH;
extern int VSCREEN_HEIGHT;

static const int blockSizeTable[2][PHASE_NUM] = {
    { 160, 80, 40, 20, 10, 5 },
    { 200, 100, 50, 25, 10, 5 }
};

TYMosaicEffectGenerater::TYMosaicEffectGenerater(QObject* parent)
    : TYEffectGenerater(parent)
    , m_inToward(false)
    , m_nowBitmapData(nullptr)
    , m_sourceBitmap(nullptr)
    , m_phaseInterval(0)
    , m_phase(0)
{
}

TYMosaicEffectGenerater::TYMosaicEffectGenerater(QImage* befImage, QImage* atImage,
                                                 TYEffectDefinitionValue* effect, QObject* parent)
    : TYEffectGenerater(parent)
    , m_inToward(false)
    , m_nowBitmapData(nullptr)
    , m_sourceBitmap(nullptr)
    , m_phaseInterval(0)
    , m_phase(0)
{
    TYEffectDefinition effdef;
    effect->getValue(&effdef);

    m_phaseInterval = effdef.time / 1000.0 / PHASE_NUM;
    //time = effdef.time;

    QTimer::singleShot(0, this, &TYMosaicEffectGenerater::drawEffect);

    m_inToward = (effdef.type == TYEffectMosaicIn);

    if (m_inToward) {
        m_sourceBitmap = atImage ? new QImage(*atImage) : nullptr;
        m_afterImage = atImage ? new QImage(*atImage) : nullptr;
    } else {
        m_sourceBitmap = befImage ? new QImage(*befImage) : nullptr;
    }

    m_nowImage = new QImage(VSCREEN_WIDTH, VSCREEN_HEIGHT, QImage::Format_RGB888);
    m_nowBitmapData = new unsigned char[VSCREEN_WIDTH * VSCREEN_HEIGHT * 3];
}

TYMosaicEffectGenerater::~TYMosaicEffectGenerater()
{
    if (m_sourceBitmap) delete m_sourceBitmap;
    if (m_nowBitmapData) delete[] m_nowBitmapData;
}

// private method
void TYMosaicEffectGenerater::drawEffect()
{
    // 経過時間をPHASE単位で取得
    //phase =  (-[ startDate timeIntervalSinceNow ] *1000) / time *PHASE_NUM;
    if (m_phase >= PHASE_NUM) {
        QTimer::singleShot(0, this, &TYEffectGenerater::effectFinished);
        if (m_inToward) {
            emit changeEffectionImage(m_afterImage);
        }
        m_phase = PHASE_NUM - 1;
        return;
    }

    QTimer::singleShot(static_cast<int>(m_phaseInterval * 1000), this, &TYMosaicEffectGenerater::drawEffect);

    if (!m_sourceBitmap)
        return;

    int blockSize, w, h;
    int wide = m_sourceBitmap->width();
    int high = m_sourceBitmap->height();

    int heightIndex = (VSCREEN_HEIGHT == 600) ? 1 : 0;
    if (m_inToward) {
        blockSize = blockSizeTable[heightIndex][m_phase];
    } else {
        blockSize = blockSizeTable[heightIndex][PHASE_NUM - m_phase - 1];
    }

    unsigned char* src = m_sourceBitmap->bits();
    unsigned char* now = m_nowBitmapData;
    int srcBytesPerLine = m_sourceBitmap->bytesPerLine();

    for(h = 0; h < VSCREEN_HEIGHT; h+=blockSize){
        for(w = 0; w < VSCREEN_WIDTH; w+=blockSize){
            // ブロックの平均値を求める
            unsigned long total[3] = {0, 0, 0};
            int count = 0;

            for (int ph = 0; ph < blockSize && (h + ph) < VSCREEN_HEIGHT; ph++) {
                int rowIdx = (h + ph) * srcBytesPerLine;
                for (int pw = 0; pw < blockSize && (w + pw) < VSCREEN_WIDTH; pw++) {
                    int pixelIdx = rowIdx + (w + pw) * 3;
                    if (pixelIdx + 2 < srcBytesPerLine * high) {
                        total[0] += src[pixelIdx];
                        total[1] += src[pixelIdx + 1];
                        total[2] += src[pixelIdx + 2];
                        count++;
                    }
                }
            }
            // ブロック内の各ピクセルに値を設定
            if (count > 0) {
                total[0] /= count;
                total[1] /= count;
                total[2] /= count;
            }
            for (int ph = 0; ph < blockSize && (h + ph) < VSCREEN_HEIGHT; ph++) {
                int idx = ((h + ph) * VSCREEN_WIDTH + w) * 3;
                for (int pw = 0; pw < blockSize && (w + pw) < VSCREEN_WIDTH; pw++) {
                    if (idx + 2 < VSCREEN_WIDTH * VSCREEN_HEIGHT * 3) {
                        now[idx] = static_cast<unsigned char>(total[0]);
                        now[idx + 1] = static_cast<unsigned char>(total[1]);
                        now[idx + 2] = static_cast<unsigned char>(total[2]);
                    }
                    idx += 3;
                }
            }
        }
    }

    memcpy(m_nowImage->bits(), m_nowBitmapData, VSCREEN_WIDTH * VSCREEN_HEIGHT * 3);

    emit changeEffectionImage(m_nowImage);

    m_phase++;
}
