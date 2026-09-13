//
//  TYSprite.m
//  Tukuyomi
//
//  Created by toveta on Fri Oct 19 2001.
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

#include "TYSprite.h"
#include "TYCellImage.h"



void TYSprite::draw()
{
    if(alpha <= 0)
        return;
    else if(alpha<SPRITE_ALPHA_MAX)
        sourceImage->copy(QPoint(0, 0)).scaled(drawPoint, Qt::IgnoreAspectRatio);
    else
        sourceImage->copy(QPoint(0, 0)).scaled(drawPoint, Qt::IgnoreAspectRatio);
}

QRect TYSprite::rect()
{
    QSize size = sourceImage->size();

    Q_ASSERT(sourceImage);

    return QRect(drawPoint.x(), drawPoint.y(), size.width(), size.height());
}

void TYSprite::loadImageFromPath()
{
    if(sourceImage)
        delete sourceImage;
    sourceImage = TYResourceServer::sharedServer()->getImage(imagePath, 
                                                             true, 
                                                             true);
}

QImage* TYSprite::sourceImage()
{
    if(!sourceImage)
        loadImageFromPath();
    return sourceImage;
}

void TYSprite::setCell(int cellNo)
{
    ((TYCellImage*)sourceImage)->changeCell(cellNo);
}

void TYSprite::convertAlphaOfFix()
{
    alpha = alpha *255 /100;
}

// implementation of NSCoding
















TYSprite::~TYSprite()
{
    if (sourceImage) delete sourceImage;
    if (!imagePath.isEmpty()) imagePath.clear();

}

