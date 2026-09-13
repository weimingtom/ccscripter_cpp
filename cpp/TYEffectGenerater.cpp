//
//  TYEffectGenerater.m
//  Tukuyomi
//
//  Created by toveta on Sun Nov 11 2001.
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

#include "TYEffectGenerater.h"
#include "TYInstantGenerater.h"
#include "TYCrossFadeGenerater.h"
#include "TYScrollEffectGenerater.h"
#include "TYShutterEffectGenerater.h"
#include "TYCartainEffectGenerater.h"
#include "TYQuakeEffectGenerater.h"
#include "TYPatternFadeGenerater.h"
#include "TYPatternCrossFadeGenerater.h"
#include "TYMosaicEffectGenerater.h"

TYEffectGenerater::TYEffectGenerater(QObject *parent)
    : QObject(parent)
    , m_delegate(nullptr)
    , m_timer(nullptr)
{
}

TYEffectGenerater::~TYEffectGenerater() {
    if (m_timer) {
        m_timer->stop();
        delete m_timer;
    }
}

void TYEffectGenerater::initWithBeforeImage(const QImage& befImage, const QImage& atImage, TYEffectDefinitionValue* effect) {
    TYEffectDefinition def;
    effect->getValue(&def);
    
    m_beforeImage = befImage;
    m_afterImage = atImage;
    
    if (def.type == TYEffectCached) {
        QTimer::singleShot(0, this, SLOT(drawEffect()));
        return;
    }
}

void TYEffectGenerater::initWithImage(const QImage& image, int type, int times, int second) {
    TYQuakeEffectGenerater *quakeGen = new TYQuakeEffectGenerater(nullptr);
    quakeGen->initWithImage(image, type, times, second);
}

void TYEffectGenerater::setDelegate(QObject *aDelegate) {
    m_delegate = aDelegate;
}

void TYEffectGenerater::drawEffect() {
    effectFinished();
}

void TYEffectGenerater::effectFinished() {
    if (m_delegate) {
        QMetaObject::invokeMethod(m_delegate, "effectFinished", Qt::DirectConnection);
    }
    emit effectFinishedSignal();
}

TYEffectGenerater* createEffectGenerater(const QImage& befImage, const QImage& atImage, TYEffectDefinitionValue* effect) {
    TYEffectDefinition def;
    effect->getValue(&def);
    
    TYEffectGenerater *generater = nullptr;
    
    switch (def.type) {
    case TYEffectInstant:
        generater = new TYInstantGenerater();
        break;
    case TYEffectLeftShutter:
    case TYEffectRightShutter:
    case TYEffectTopShutter:
    case TYEffectBottomShutter:
        generater = new TYShutterEffectGenerater();
        break;
    case TYEffectLeftCartain:
    case TYEffectRightCartain:
    case TYEffectTopCartain:
    case TYEffectBottomCartain:
        generater = new TYCartainEffectGenerater();
        break;
    case TYEffectRightScroll:
    case TYEffectLeftScroll:
    case TYEffectBottomScroll:
    case TYEffectTopScroll:
        generater = new TYScrollEffectGenerater();
        break;
    case TYEffectMosaicOut:
    case TYEffectMosaicIn:
        generater = new TYMosaicEffectGenerater();
        break;
    case TYEffectFadeWithMask:
        {
            TYPatternFadeGenerater *patternFade = new TYPatternFadeGenerater();
            patternFade->initWithBeforeImage(befImage, atImage, effect);
            if (patternFade) {
                generater = patternFade;
            } else {
                generater = new TYCrossFadeGenerater();
            }
        }
        break;
    case TYEffectCrossFadeWithMask:
        {
            TYPatternCrossFadeGenerater *patternCrossFade = new TYPatternCrossFadeGenerater();
            patternCrossFade->initWithBeforeImage(befImage, atImage, effect);
            if (patternCrossFade) {
                generater = patternCrossFade;
            } else {
                generater = new TYCrossFadeGenerater();
            }
        }
        break;
    default:
        generater = new TYCrossFadeGenerater();
    }
    
    if (generater) {
        generater->initWithBeforeImage(befImage, atImage, effect);
    }
    
    return generater;
}
