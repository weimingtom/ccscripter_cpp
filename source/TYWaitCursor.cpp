//
//  TYWaitCursor.m
//  Tukuyomi
//
//  Created by toveta on Sat Mar 09 2002.
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

#import "TYWaitCursor.h"
#import "TYStageManager.h"

@implementation TYWaitCursor

-(id)initWithResource:(NSString*)aPath offset:(NSPoint)point absolute:(BOOL)aBool
{
    self = [ super init ];
    if(![ aPath length ]){
        [ self autorelease ];
        return nil;
    }

    resourcePath = [ aPath retain ];
    offset = point;
    isAbsolute = aBool;
    
    return self;
}

-(void)loadImage
{
    if(isLoad)
        return;
    isLoad = YES;

    cursorImage = [ [ TYResourceServer sharedServer ] getImage:resourcePath transMode:YES animate:YES ];
    if(!cursorImage)
        return ;
    [ cursorImage retain ];
    if([ cursorImage isAnimate ]){
        [ (TYAnimationCellImage*)cursorImage setDelegate:self ];
    }
    cursorSize = (cursorImage) ? [ cursorImage size ] : NSZeroSize ;

    bufferImage = [ [ NSImage alloc ] initWithSize:cursorSize ];
}

-(void)setDelegate:(id)aObject
{
    delegate = aObject;
}

-(id)compositeLayer
{
    return [ delegate compositeLayer ];
}

-(void)drawToView:(id)aView point:(NSPoint)point
{
    targetView = aView;

    [ self loadImage ];
    if(!cursorImage)
        return;

    if(isAbsolute){
        targetPoint = NSMakePoint(offset.x,VSCREEN_HEIGHT-offset.y-[ cursorImage size ].height);
    } else {
        targetPoint = point;
        targetPoint.x += offset.x;
        targetPoint.y += -offset.y;
        targetPoint.y += -[ cursorImage size ].height;
    }


    if([ (TYAnimationCellImage*)cursorImage isAnimate ]&&(![ (TYAnimationCellImage*)cursorImage isPlaying ])){
        [ (TYAnimationCellImage*)cursorImage play ];
    }

    [ self drawImage:YES ];
}


-(void)didChangeCell:(id)sender
{
    [ self drawImage:YES ];
}

-(void)drawImage:(BOOL)aBool
{
    NSImage *offscreen;
    NSRect inRect = NSMakeRect(targetPoint.x,
                               (isAbsolute) ? VSCREEN_HEIGHT -offset.y -cursorSize.height : targetPoint.y ,
                               cursorSize.width,
                               cursorSize.height);
    
    // オフスクリーンを取得
    offscreen = [ delegate compositeLayer ];

    // バッファにコピー
    [ bufferImage lockFocus ];
    NSEraseRect(NSMakeRect(0,0,inRect.size.width,inRect.size.height));
    [ offscreen compositeToPoint:NSZeroPoint
                        fromRect:inRect
                       operation:NSCompositeCopy ];
     
    if(aBool){
        //[ offscreen lockFocus ];
        [ cursorImage compositeToPoint:NSZeroPoint operation:NSCompositeSourceOver ];
        //[ offscreen unlockFocus ];
    }
    [ bufferImage unlockFocus ];

    // ウインドウに描画
     [ targetView directDrawImage:bufferImage
                          inRect:inRect
                        fromRect:NSMakeRect(0,0,cursorSize.width,cursorSize.height) ];
    //[ targetView setImage:offscreen ];

}

-(void)clear
{
    if(!cursorImage)
        return;

    [ self stop ];
    
    [ targetView setImage:[ delegate compositeLayer ] ];
    //[ self drawImage:NO ];
}

-(void)stop
{
    if(cursorImage && [ (TYAnimationCellImage*)cursorImage isAnimate ]&&[ (TYAnimationCellImage*)cursorImage isPlaying ]){
        [ (TYAnimationCellImage*)cursorImage stop ];
    }    
}

-(void)resume
{
    if(cursorImage && [ (TYAnimationCellImage*)cursorImage isAnimate ])
        [ (TYAnimationCellImage*)cursorImage resume ];
}

- (void)dealloc
{
    [ cursorImage release ];
    [resourcePath release];
    [ bufferImage release ];
    [super dealloc];
}

-(NSString*)description
{
    return [ NSString stringWithFormat:@"path=%@ point=%@ absolute=%d",resourcePath,NSStringFromPoint(offset),isAbsolute ];
}

// ----------------------------------------------------------------------------------------
// NSCoding
// ----------------------------------------------------------------------------------------

- (id)initWithCoder:(NSCoder *)aDecoder
{
    [super init];
    resourcePath = [ [ aDecoder decodeObject ] retain ];
    offset = [ aDecoder decodePoint ];
    [ aDecoder decodeValueOfObjCType:@encode(BOOL) at:&isAbsolute ];

    return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
    [ aCoder encodeObject:resourcePath ];
    [ aCoder encodePoint:offset ];
    [ aCoder encodeValueOfObjCType:@encode(BOOL) at:&isAbsolute ];

}

@end
