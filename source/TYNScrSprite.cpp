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

#import "TYNScrSprite.h"
#import "TYStageManager.h"
#import "StopNSLog.h"

NSString* const TYSpriteAnimationNotification=@"TYSpriteAnimationNotification";

@implementation TYNScrSprite

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

-(id)initWithID:(int)idNo path:(NSString*)aPath point:(NSPoint)point alpha:(NSNumber*)alphaNum visible:(BOOL)aBool;
{
    NSSize imageSize;

    self = [ super init ];

    spriteID = idNo;
    imagePath = [ aPath retain ];

    visible = aBool;
    [ self loadImageFromPath ];

    imageSize = (sourceImage) ? [ sourceImage size ] : NSZeroSize;

    drawPoint.x = point.x;
    drawPoint.y = VSCREEN_HEIGHT -imageSize.height -point.y;

    if(alphaNum)
        alpha = [ alphaNum intValue ];

    /*
    if(visible && sourceImage && [ (TYAnimationCellImage*)sourceImage isAnimate ]){
        [ (TYAnimationCellImage*)sourceImage play ];
        [ sourceImage setDelegate:self ];
    }
     */
    
    return self;
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

-(void)moveX:(NSNumber*)x Y:(NSNumber*)y alpha:(NSNumber*)alphaNum
{
    //NSSize imageSize;
    
    drawPoint.x += [ x intValue ];
    drawPoint.y -= [ y intValue ]; // MacではY座標軸が反転している。
    alpha += [ alphaNum intValue ];
    if(alpha < 0)
        alpha = 0;
    else if(alpha > SPRITE_ALPHA_MAX)
        alpha = SPRITE_ALPHA_MAX;
        
    //imageSize = [ sourceImage size ];
}

-(void)absoluteMoveX:(NSNumber*)x Y:(NSNumber*)y alpha:(NSNumber*)alphaNum
{
    drawPoint.x = [ x intValue ];
    drawPoint.y = VSCREEN_HEIGHT -[ sourceImage size ].height - [ y intValue ];
    if(alphaNum){
        alpha = [ alphaNum intValue ];
        if(alpha < 0)
            alpha = 0;
        else if(alpha > SPRITE_ALPHA_MAX)
            alpha = SPRITE_ALPHA_MAX;
    }
}

-(void)setVisible:(BOOL)aBool
{
    if(visible == aBool) {
        return;
    }
    
    visible = aBool;
    if(sourceImage && [ (TYAnimationCellImage*)sourceImage isAnimate ]){
        if(visible && ![ (TYAnimationCellImage*)sourceImage isPlaying ]){
            [ (TYAnimationCellImage*)sourceImage play ];
            [ sourceImage setDelegate:self ];
        } else if(!visible && [ (TYAnimationCellImage*)sourceImage isPlaying ]){
            [ (TYAnimationCellImage*)sourceImage stop ];
        }
    }    
}
-(BOOL)visible { return visible; }

-(int)spriteID
{
    return spriteID;
}

-(void)draw // override
{    
    if(visible)
        [ super draw ];
}

-(void)loadImageFromPath
{
    [ super loadImageFromPath ];

    // MEMO:立ち絵のアニメ対応を実装した場合はこのコードはスーパークラスに移る。
    if(sourceImage && [ sourceImage isAnimate ] && visible){
        [ (TYAnimationCellImage*)sourceImage play ];
        [ sourceImage setDelegate:self ];
    }
}

/*
-(BOOL)isEqual:(id)object // BASIC protocol
{
    if(![ [ self class ] isMemberOfClass:[ object class ] ]){
        return NO;
    }
    
    return ([ self spriteID ] == [ object spriteID ]) ? YES : NO;
}
*/


// implementation of NSCoding
- (void)encodeWithCoder:(NSCoder *)aCoder
{
    [ super encodeWithCoder:aCoder ];
    
    [ aCoder encodeValueOfObjCType:"i" at:&spriteID ];
    [ aCoder encodeValueOfObjCType:@encode(BOOL) at:&visible ];
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
    [ super initWithCoder:aDecoder ];
    [ aDecoder decodeValueOfObjCType:"i" at:&spriteID ];
    [ aDecoder decodeValueOfObjCType:@encode(BOOL) at:&visible ];

    return self;
}

-(NSString*)description
{
    return [ NSString stringWithFormat:@"ID=%d,point=%@,alpha=%d",spriteID,NSStringFromPoint(drawPoint),alpha ];
}

-(void)didChangeCell:(id)sender
{
    [ [ NSNotificationCenter defaultCenter ] postNotificationName:TYSpriteAnimationNotification object:self ];
}

- (void)dealloc
{
    // MEMO:立ち絵のアニメ対応を実装した場合はこのコードはスーパークラスに移る。
    if(sourceImage && [ sourceImage isAnimate ] && [ (TYAnimationCellImage*)sourceImage isPlaying ]){
        [ (TYAnimationCellImage*)sourceImage stop ];
    }
    [super dealloc];
}

@end
