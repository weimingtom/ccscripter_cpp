//
//  TYCellImage.m
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

#include "TYCellImage.h"

TYCellImage::TYCellImage()
{
    currentCell = 0;
    currentImage = nullptr;
}

TYCellImage::TYCellImage(QList<QImage*> aArray)
{
    imageArray = aArray;
    currentCell = 0;
    
    if(!imageArray.isEmpty())
        currentImage = new QImage(*imageArray.at(0));
}

TYCellImage::~TYCellImage()
{
    qDeleteAll(imageArray);
    if(currentImage)
        delete currentImage;
}

void TYCellImage::changeCell(int index)
{
    currentCell = index;
    
    if(currentImage)
        delete currentImage;
    currentImage = new QImage(*imageArray.at(index));
}

int TYCellImage::currentCell()
{
    return currentCell;
}

int TYCellImage::cells()
{
    return imageArray.count();
}

bool TYCellImage::isCellImage()
{
    return true;
}

bool TYCellImage::isAnimate()
{
    return false;
}

QString TYCellImage::description()
{
    return QString("TYCellImage current=%1,images=%2").arg(currentCell).arg(imageArray.size());
}
