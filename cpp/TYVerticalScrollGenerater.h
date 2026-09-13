//
//  TYVerticalScrollGenerater.h
//  Tukuyomi
//
//  Created by toveta on Fri Nov 30 2001.
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

#ifndef TYVerticalScrollGenerater_h
#define TYVerticalScrollGenerater_h

#include "TYEffectGenerater.h"
#include <QObject>
#include <QImage>
#include <QTime>

class TYVerticalScrollGenerater : public TYEffectGenerater {
    Q_OBJECT
private:
    QImage unionImage;
    bool reverse;
    QTime startDate;
    double phaseInterval;
    int time;
public:
    void initWithBeforeImage(QImage befImage, QImage atImage, TYEffectDefinition effdef, bool aBool);
protected:
// "Up To Scrool" is NO ,"Down To" is YES

// private method
    void drawEffect(); // override
public:
    TYVerticalScrollGenerater() : TYEffectGenerater()
    , reverse(false)
    , phaseInterval(0.0)
    , time(0)
    {
    }
    virtual ~TYVerticalScrollGenerater();
};

#endif // TYVerticalScrollGenerater_h
