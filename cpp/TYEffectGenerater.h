//
//  TYEffectGenerater.h
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

#ifndef TYEFFECTGENERATER_H
#define TYEFFECTGENERATER_H

#include <QObject>
#include <QImage>
#include <QTimer>

#include "TYEffecter.h"

// 実際にエフェクト効果をかけたイメージを作成するオブジェクトのクラスクラスター
class TYEffectGenerater : public QObject {
    Q_OBJECT
public:
    explicit TYEffectGenerater(QObject *parent = nullptr);
    virtual ~TYEffectGenerater();
    
    virtual void initWithBeforeImage(const QImage& befImage, const QImage& atImage, TYEffectDefinitionValue* effect);
    virtual void initWithImage(const QImage& image, int type, int times, int second);
    void setDelegate(QObject *aDelegate);
    
public slots:
// private method
    virtual void drawEffect(); // need to override from subclasses
    void effectFinished();
    
signals:
    void changeEffectionImage(const QImage& drawImage);
    void effectFinishedSignal();
    
protected:
    QObject *m_delegate;
    QImage m_beforeImage;
    QImage m_afterImage;
    QImage m_nowImage;
    QTimer *m_timer;
};

TYEffectGenerater* createEffectGenerater(const QImage& befImage, const QImage& atImage, TYEffectDefinitionValue* effect);

#endif // TYEFFECTGENERATER_H
