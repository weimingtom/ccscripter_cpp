//
//  TYCartainEffectGenerater.m
//  Tukuyomi
//
//  Created by toveta on Sun Dec 02 2001.
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

#include "TYCartainEffectGenerater.h"
#include "TYStageManager.h"

#define PIXEL_OF_COLUMN 24

TYCartainEffectGenerater::TYCartainEffectGenerater(QObject *parent)
    : TYEffectGenerater(parent)
    , m_horizontal(false)
    , m_reverse(false)
    , m_phaseInterval(0)
    , m_time(0)
    , m_numberOfColumn(0)
    , m_beforePhase(0)
    , m_phaseMAX(0)
    , m_pointMAX(0)
    , m_remainder(0)
{
}

TYCartainEffectGenerater::~TYCartainEffectGenerater() {
}

void TYCartainEffectGenerater::initWithBeforeImage(const QImage& befImage, const QImage& atImage, TYEffectDefinitionValue* effect) {
    TYEffectDefinition effdef;
    effect->getValue(&effdef);
    
    switch (effdef.type) {
    case TYEffectTopCartain:
        m_reverse = true;
        break;
    case TYEffectRightCartain:
        m_reverse = true;
        // fall through
    case TYEffectLeftCartain:
        m_horizontal = true;
        break;
    default:
        break;
    }
    
    if (m_horizontal) {
        m_numberOfColumn = VSCREEN_WIDTH / PIXEL_OF_COLUMN;
        m_phaseMAX = PIXEL_OF_COLUMN + m_numberOfColumn - 1;
        m_pointMAX = VSCREEN_WIDTH;
        m_remainder = VSCREEN_WIDTH % PIXEL_OF_COLUMN;
    } else {
        m_numberOfColumn = VSCREEN_HEIGHT / PIXEL_OF_COLUMN;
        m_phaseMAX = PIXEL_OF_COLUMN + m_numberOfColumn - 1;
        m_pointMAX = VSCREEN_HEIGHT;
    }
    
    m_phaseInterval = effdef.time / 1000.0 / m_phaseMAX;
    m_time = effdef.time;
    
    QTimer::singleShot(0, this, SLOT(drawEffect()));
    
    m_afterImage = atImage;
    m_nowImage = befImage;
    
    m_startDate = QDateTime::currentDateTime();
    
    m_beforePhase = 0;
}

void TYCartainEffectGenerater::drawEffect() {
    // 経過時間をフェイズ単位で取得
    int phase = (-m_startDate.msecsTo(QDateTime::currentDateTime())) / m_time * m_phaseMAX;
    if (phase >= m_phaseMAX) {
        emit changeEffectionImage(m_afterImage);
        QTimer::singleShot(0, this, SLOT(effectFinished()));
        return;
    }
    
    QTimer::singleShot(static_cast<int>(m_phaseInterval * 1000), this, SLOT(drawEffect()));
    
    int movePhase = phase - m_beforePhase;
    // まあ、ありえないとは思うが。
    if(movePhase==0){
        return;
    }
    
    int x, y, w, h;
    int currentPhase;
    int i, loc;
    int *mp;
    
    if (m_horizontal) {
        w = 1;
        h = VSCREEN_HEIGHT;
        y = 0;
        mp = &x;
    } else {
        w = VSCREEN_WIDTH;
        h = 1;
        x = 0;
        mp = &y;
    }
    
    QImage resultImage = m_nowImage;
    
    for (currentPhase = m_beforePhase + 1; currentPhase <= phase; currentPhase++) {
        for (i = 0, loc = currentPhase - 1; i < m_numberOfColumn; i++, loc--) {
            if ((loc >= 0) && (loc < PIXEL_OF_COLUMN)) {
                *mp = m_reverse ? m_pointMAX - PIXEL_OF_COLUMN * i - loc - 1 : PIXEL_OF_COLUMN * i + loc;
                
                QRect srcRect(x, y, w, h);
                QRect dstRect(x, y, w, h);
            }
        }
        // 余りがあればその処理
        if (m_remainder) {
            if ((loc >= 0) && (loc < m_remainder)) {
                *mp = m_reverse ? m_pointMAX - PIXEL_OF_COLUMN * i - loc - 1 : PIXEL_OF_COLUMN * i + loc;
            }
        }
    }
    
    m_nowImage = resultImage;
    emit changeEffectionImage(m_nowImage);
    
    m_beforePhase = phase;
}
