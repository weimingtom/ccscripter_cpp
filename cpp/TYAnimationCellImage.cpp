//
//  TYAnimationCellImage.cpp
//  Tukuyomi
//
//  Created by toveta on Wed Feb 27 2002.
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

#include "TYAnimationCellImage.h"
#include "StopNSLog.h"

QString TYAnimationModeInfo = "TYAnimationModeInfo"; // number of constant
QString TYAnimationIntervalInfo = "TYAnimationIntervalInfo"; // number or numbers in Array

TYAnimationCellImage::TYAnimationCellImage()
{
    isPlaying = false;
    back = false;
    intervalIsArray = false;
    delegate = nullptr;
    interval = nullptr;
    mode = TYAnimationNoPlay;
}

TYAnimationCellImage::TYAnimationCellImage(QList<QImage*> aArray, QVariantMap aDict)
    : TYCellImage(aArray)
{
    // アニメ指定判定
    mode = (TYAnimationMode)aDict.value(TYAnimationModeInfo).toInt();

    // タイマ作成
    if(mode != TYAnimationNoPlay){
        interval = aDict.value(TYAnimationIntervalInfo);
    }
}

void TYAnimationCellImage::setDelegate(QObject* aObject)
{
    delegate = aObject;
}

void TYAnimationCellImage::changingCell()
{
    if(!isPlaying)
        return;
    
    switch(mode){
        case TYAnimationLoopingPlayback:
            if(currentCell + 1 >= cells()){
                changeCell(0);
            } else {
                changeCell(currentCell + 1);
            }
            setNextTimer();
            break;
        case TYAnimationNormalPlayback:
            if(currentCell + 1 == cells()){
                stop();
            } else {
                changeCell(currentCell + 1);
                setNextTimer();
            }
            break;
        case TYAnimationLoopingBackAndForthPlayback:
            if(!back){
                if(currentCell + 1 == cells() - 1){
                    back = true;
                }
                changeCell(currentCell + 1);
            } else {
                if(currentCell == 1){
                    back = false;
                }
                changeCell(currentCell - 1);
            }
            setNextTimer();
            break;
        default:
            break;
    }

    /*
    if([ self currentCell ] +1 > [ self cells ]){
        [ self changeCell:0 ];
    } else {
        [ self changeCell:[ self currentCell ]+1 ];
    }
     */

    didChangeCell(this);
}

// implemantation for delegate
void TYAnimationCellImage::didChangeCell(QObject* sender)
{
    if(delegate && delegate->property("didChangeCell").isValid()){
        QMetaObject::invokeMethod(delegate, "didChangeCell", Q_ARG(QObject*, this));
    }
}

// animation operate
void TYAnimationCellImage::play()
{
    if(mode==TYAnimationNoPlay)
        return;

    back = false;
    changeCell(0);
    resume();
}

bool TYAnimationCellImage::isPlaying()
{
    return isPlaying;
}

void TYAnimationCellImage::stop()
{
    // タイマストップ
    isPlaying = false;
}

void TYAnimationCellImage::resume()
{
    
    if(mode == TYAnimationNoPlay)
        return;
    isPlaying = true;

    setNextTimer();
}

void TYAnimationCellImage::setNextTimer()
{
    int delay;

    // タイマ起動
    if(interval.userType() == QMetaType::Int){
        delay = interval.toInt();
    } else {
        QVariantList intervalList = interval.toList();
        delay = intervalList.at(currentCell).toInt();
    }

    QTimer::singleShot(delay / 1000.0 * 1000, this, &TYAnimationCellImage::changingCell);
}

bool TYAnimationCellImage::isAnimate()
{
    return true;
}
