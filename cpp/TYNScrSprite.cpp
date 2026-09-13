//
//  TYNScrSprite.m
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

#include "TYNScrSprite.h"
#include "TYStageManager.h"
#include "StopNSLog.h"

QString TYSpriteAnimationNotification = "TYSpriteAnimationNotification";

TYNScrSprite::TYNScrSprite()
{
}

TYNScrSprite::~TYNScrSprite()
{
}
/*
+(id)dummySprite:(int)idNo // Array検索用のダミー
{
    return [ [ [ [ self class ] alloc ] initWithID:idNo
                                        image:nil
                                        point:NSZeroPoint
                                        alpha:[ NSNumber numberWithInt:0 ]
                                        visible:NO ] autorelease ];
}
*/

TYNScrSprite* TYNScrSprite::initWithID(int idNo, QString aPath, QPoint point, int alphaNum, bool aBool)
{
    QSize imageSize;

    spriteID = idNo;
    imagePath = aPath;

    visible = aBool;
    loadImageFromPath();

    imageSize = (sourceImage) ? sourceImage.size() : QSize(0, 0);

    drawPoint.setX(point.x());
    drawPoint.setY(VSCREEN_HEIGHT - imageSize.height() - point.y());

    if(alphaNum)
        alpha = alphaNum;

    return this;
}
/*
-(id)initWithID:(int)idNo image:(NSImage*)aImage point:(NSPoint)point alpha:(NSNumber*)alphaNum visible:(BOOL)aBool
{
    NSSize imageSize;
    NSSize vSize;

    self = [ super init ];
    
    spriteID = idNo;
    
    sourceImage = aImage;
    [ sourceImage retain ];
    
    // 描画地点、領域を求める。
    imageSize = (aImage) ? [ aImage size ] : NSZeroSize;

    drawPoint.x = point.x;
    drawPoint.y = VSCREEN_HEIGHT -imageSize.height -point.y;
    
    alpha = [ alphaNum intValue ];
    
    visible = aBool;
    
    return self;
}
*/

void TYNScrSprite::moveX(int x, int y, int alphaNum)
{
    //NSSize imageSize;
    
    drawPoint.setX(drawPoint.x() + x);
    drawPoint.setY(drawPoint.y() - y); // MacではY座標軸が反転している。
    alpha += alphaNum;
    if(alpha < 0)
        alpha = 0;
    else if(alpha > SPRITE_ALPHA_MAX)
        alpha = SPRITE_ALPHA_MAX;
        
    //imageSize = [ sourceImage size ];
}

void TYNScrSprite::absoluteMoveX(int x, int y, int alphaNum)
{
    drawPoint.setX(x);
    drawPoint.setY(VSCREEN_HEIGHT - sourceImage.size().height() - y);
    if(alphaNum){
        alpha = alphaNum;
        if(alpha < 0)
            alpha = 0;
        else if(alpha > SPRITE_ALPHA_MAX)
            alpha = SPRITE_ALPHA_MAX;
    }
}

void TYNScrSprite::setVisible(bool aBool)
{
    if(visible == aBool) {
        return;
    }
    
    visible = aBool;
    if(sourceImage && ((TYAnimationCellImage*)sourceImage)->isAnimate()){
        if(visible && !((TYAnimationCellImage*)sourceImage)->isPlaying()){
            ((TYAnimationCellImage*)sourceImage)->play();
            sourceImage->setDelegate(this);
        } else if(!visible && ((TYAnimationCellImage*)sourceImage)->isPlaying()){
            ((TYAnimationCellImage*)sourceImage)->stop();
        }
    }    
}

bool TYNScrSprite::visible()
{
    return visible;
}

int TYNScrSprite::spriteID()
{
    return spriteID;
}

void TYNScrSprite::draw()
{
    if(visible)
        TYSprite::draw();
}

void TYNScrSprite::loadImageFromPath()
{
    TYSprite::loadImageFromPath();

    // MEMO:立ち絵のアニメ対応を実装した場合はこのコードはスーパークラスに移る。
    if(sourceImage && sourceImage->isAnimate() && visible){
        ((TYAnimationCellImage*)sourceImage)->play();
        sourceImage->setDelegate(this);
    }
}

QString TYNScrSprite::description()
{
    return QString("ID=%1,point=(%2,%3),alpha=%4").arg(spriteID).arg(drawPoint.x()).arg(drawPoint.y()).arg(alpha);
}

void TYNScrSprite::didChangeCell(QObject* sender)
{
    Q_EMIT TYSpriteAnimationNotificationChanged(this);
}

TYNScrSprite::~TYNScrSprite()
{
    // MEMO:立ち絵のアニメ対応を実装した場合はこのコードはスーパークラスに移る。
    if(sourceImage && sourceImage->isAnimate() && ((TYAnimationCellImage*)sourceImage)->isPlaying()){
        ((TYAnimationCellImage*)sourceImage)->stop();
    }
}
