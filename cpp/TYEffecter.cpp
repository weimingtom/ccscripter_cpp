//
//  TYEffecter.m
//  Tukuyomi
//
//  Created by toveta on Mon Jun 18 2001.
//  Copyright (c) 2001 toveta. All rights reserved.
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

#include "TYEffecter.h"
#include <QDebug>

TYEffecter::TYEffecter(QObject* parent)
    : QObject(parent)
    , currentEffectNo_(0)
    , currentTime_(0)
{
}

TYEffecter::~TYEffecter()
{
}

TYEffecter* TYEffecter::effecter()
{
    static TYEffecter instance;
    return &instance;
}

void TYEffecter::setEffectNo(int effectNo, int effectType, int time, const QString& path)
{
    currentEffectNo_ = effectNo;
    currentTime_ = time;
    currentPath_ = path;
    Q_UNUSED(effectType);
}

void TYEffecter::setBeforeImage(QImage aImage)
{
    beforeImage_ = aImage;
}

QImage TYEffecter::beforeImage() const
{
    return beforeImage_;
}

bool TYEffecter::startEffect(QImage newImage, int effectNo)
{
    afterImage_ = newImage;
    Q_UNUSED(effectNo);
    return true;
}

bool TYEffecter::getEffectionImage(QImage& drawImage)
{
    drawImage = nowImage_;
    return true;
}
