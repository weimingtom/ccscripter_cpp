//
//  TYStandingChar.m
//  Tukuyomi
//
//  Created by toveta on Tue Jan 08 2002.
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

#include "TYStandingChar.h"
#include "TYStageManager.h"


int TYStandingChar::underOffset = 0;

void TYStandingChar::setUnderline(int aInt)
{
    underOffset = aInt;
}

int TYStandingChar::underline()
{
    return underOffset;
}

TYStandingChar* TYStandingChar::initWithPosition(QString str, QString aPath)
{
    QSize imageSize;
    
    TYSprite::TYSprite();

    position = str;
    
    imagePath = aPath;

    loadImageFromPath();

    imageSize = sourceImage ? sourceImage->size() : QSize(0, 0);
    drawPoint.setY(0);
    if(str == LD_LOCATE_LEFT){
        drawPoint.setX(VSCREEN_WIDTH / 4 - imageSize.width() / 2);
    } else if(str == LD_LOCATE_CENTER){
        drawPoint.setX(VSCREEN_WIDTH / 2 - imageSize.width() / 2);
    } else if(str == LD_LOCATE_RIGHT){
        drawPoint.setX(VSCREEN_WIDTH * 3 / 4 - imageSize.width() / 2);
    } else {
        return nullptr;
    }

    alpha=SPRITE_ALPHA_MAX;
    
    return this;
}

void TYStandingChar::draw()
{
    drawPoint.setY(drawPoint.y() + underOffset);
    TYSprite::draw();
    drawPoint.setY(drawPoint.y() - underOffset);
}


TYStandingChar::~TYStandingChar()
{


}

// ----------------------------------------------------------------------------------------
// NSCoding
// ----------------------------------------------------------------------------------------
