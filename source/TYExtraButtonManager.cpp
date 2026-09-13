//
//  TYExtraButtonManager.m
//  Tukuyomi
//
//  Created by toveta on Tue Jun 18 2002.
//  Copyright (c) 2002 toveta All rights reserved.
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

#import "TYExtraButtonManager.h"
#import "TYNScrSprite.h"

@implementation TYExtraButtonManager

-(id)initWithStageManager:(TYStageManager*)manager sourceImage:(NSImage*)sImage
{
    [ super initWithStageManager:manager ];

    sourceImage = [ sImage retain ];

    return self;
}

-(void)addButton:(NSNumber*)no x:(NSNumber*)x y:(NSNumber*)y width:(NSNumber*)width height:(NSNumber*)height selx:(NSNumber*)selx sely:(NSNumber*)sely
{
    NSImage *selImage;
    int w,h,sx,sy;
    NSSize srcSize;
    NSRect rect;

    isOverlayButton = YES;
    
    w = [ width intValue ];
    h = [ height intValue ];
    sx = [ selx intValue ];
    sy = [ sely intValue ];
    rect = NSMakeRect([ x intValue ],VSCREEN_HEIGHT -[ y intValue ] -h,w,h);

    // 選択状態のイメージを作成
    selImage = [ [ [ NSImage alloc ] initWithSize:NSMakeSize(w,h) ] autorelease ];
    srcSize = [ sourceImage size ];
    [ selImage lockFocus ];
    [ sourceImage compositeToPoint:NSZeroPoint fromRect:NSMakeRect(sx,srcSize.height -h -sy,w,h) operation:NSCompositeSourceOver ];

    [ selImage unlockFocus ];

    // ボタン定義の追加
    [ self addSelection:[ [ [ TYButton alloc ] initWithID:no
                                            selectedImage:selImage rect:rect ] autorelease ]
                   rect:rect ];
}

-(void)addSpriteButton:(NSNumber*)no spriteNo:(NSNumber*)sNo  appendAction:(NSString*)aString
{
    NSRect rect;

    isSpriteButton = YES;

    // スプライトのrectを取得
    rect = [ stageManager rectOfSprite:[ sNo intValue ] ];
    
    if(NSEqualRects(rect,NSZeroRect))
        return;

    // ボタン定義の追加
    [ self addSelection:[ [ [ TYSpriteButton alloc ] initWithID:no
                                                       spriteNo:sNo
                                                   appendAction:aString ] autorelease ]
                   rect:rect ];

}

-(void)addCselButton:(NSNumber*)no selectionNo:(NSNumber*)sNo image:(TYCellImage*)image point:(NSPoint)aPoint;
{
    NSRect rect;
    
    isCustomSel = YES;
    
    // ポイントの座標をMacに合わせて変換。
    rect.size = [ (NSImage*)image size ];
    aPoint.y = VSCREEN_HEIGHT -aPoint.y - rect.size.height;     
    rect.origin = aPoint;
    
    [ self addSelection:[ [ [ TYCselButton alloc ] initWithID:no
                                                   selctionNo:[ sNo intValue ]
                                                        image:image
                                                        point:aPoint ] autorelease ]
                   rect:rect ];
}

-(void)setNoSelectAction:(NSString*)aStr
{
    noSelectAction = [ aStr retain ]; 
}

-(void)initialButtonImage
{
    // ボタンモード開始前の、ボタン画像の初期化
    NSEnumerator *enm;
    id temp;

    // 画像ボタンがあり、バッファが初期化されていなければ、全てのボタンを描く
    if((isCustomSel || isOverlayButton) && beforeSelection==TYButtonUnInitialize){
        [ drawBuffer lockFocus ];
        enm = [ btnArray objectEnumerator ];
        while( (temp = [ enm nextObject ]) != nil){
            if([ temp respondsToSelector:@selector(drawWithSelect:) ])
                [ temp drawWithSelect:NO ];
        }
        [ drawBuffer unlockFocus ];
    }
    // スプライトがあればオフスクリーンを描き直し、なければ直接描画
    /*
    if(!isSpriteButton){
        [ stageManager updateStageOnlyButton ];
    } else {
        [ stageManager updateStage ];
    }
     */
    [ stageManager updateStage ];
        
    beforeSelection = TYButtonNoSelected;
}

-(BOOL)changeSelection:(int)index
{
    id exitBtn , enterBtn;
    BOOL exitIsOverlay,enterIsOverlay;

    if(beforeSelection==index){
        return NO;
    }

    exitBtn = (beforeSelection) ? [ btnArray objectAtIndex:(beforeSelection -1) ] : nil;
    enterBtn = (index) ? [ btnArray objectAtIndex:(index -1) ]: nil;

    // オーバーレイボタン(非スプライトボタン)フラグ
    exitIsOverlay = ([ exitBtn isMemberOfClass:[ TYSpriteButton class ] ]) ? NO : YES;
    enterIsOverlay = ([ enterBtn isMemberOfClass:[ TYSpriteButton class ] ]) ? NO : YES;

    if(isCustomSel & (exitIsOverlay || enterIsOverlay)){
        // 選択肢が含まれる場合は全ての選択肢とボタンを描き直し。
        TYButton *temp;
        NSEnumerator *enu = [ btnArray objectEnumerator ];
        
        [ drawBuffer lockFocus ];
        [ [ NSColor clearColor ] set ];
        NSRectFillUsingOperation(TYVirtualScreenRect(),NSCompositeClear);
        {
            while(temp = [ enu nextObject ]){
                if([ temp respondsToSelector:@selector(drawWithSelect:) ]){
                    [ temp drawWithSelect:(temp == enterBtn) ? YES : NO ];
                }
            }
        }
        [ drawBuffer unlockFocus ];
        
    } else if(exitIsOverlay || enterIsOverlay){
        [ drawBuffer lockFocus ];
        {
            // 前回選択状態だったボタンを非選択状態で描く
            if(exitBtn && exitIsOverlay){
                [ exitBtn drawWithSelect:NO ];
            }
    
            // 今回選択されたボタンを選択状態で描く
            if(enterBtn && enterIsOverlay){
                [ enterBtn drawWithSelect:YES ];
            }
        }
        [ drawBuffer unlockFocus ];
    }

    if(exitIsOverlay && enterIsOverlay){
        [ stageManager updateStageOnlyButton ];
    } else {
        if(exitBtn && !exitIsOverlay){
            [ stageManager setSprite:[ exitBtn spriteNo ] cell:0 ];
            if(!enterBtn){
                if(noSelectAction)
                    [ stageManager ty_spstr:[ NSMutableArray arrayWithObjects:@"spstr",noSelectAction,nil ] ];
            }
        }

        if(enterBtn && !enterIsOverlay){
            NSString *str;
            [ stageManager setSprite:[ enterBtn spriteNo ] cell:1 ];
            str = [ enterBtn appendString ];
            if(str)
                [ stageManager ty_spstr:[ NSMutableArray arrayWithObjects:@"spstr",str,nil ] ];
        }
        
        
        [ stageManager updateStage ];
    }
        
    //[ drawBuffer compositeToPoint:NSZeroPoint operation:NSCompositeSourceOver ];
    beforeSelection = index;

    return YES;
}

-(int)selectedButtonID // overwrite
{
    if(beforeSelection) {
        return [ [ [ btnArray objectAtIndex:(beforeSelection -1) ] idNo ] intValue ];
    } else {
        return 0;
    }
}

-(NSImage*)sourceImage { return sourceImage; }

-(void)clear
{
    [ super clear ];
    
    [ noSelectAction release ];
    noSelectAction = nil;
    isOverlayButton = NO;
    isSpriteButton = NO;
    isCustomSel = NO;
}

- (void)dealloc
{
    [sourceImage release];
    [noSelectAction release];
    [super dealloc];
}



@end

@implementation TYButton

-(id)initWithID:(NSNumber*)no selectedImage:selImage rect:(NSRect)aRect
{
    self = [ super init ];

    idno = [ no retain ];
    selectedImage = selImage;
    [ selectedImage retain ];
    drawRect = aRect;

    return self;
}

-(void)drawWithSelect:(BOOL)aBool
{
    // 自身のRectをクリアしてから描画
    [ [ NSColor whiteColor ] set ];
    NSRectFillUsingOperation(drawRect,NSCompositeClear);
    /*
     [ [ NSColor clearColor ] set ];
     [ NSBezierPath fillRect:drawRect ];
     */

    if(aBool){
        [ selectedImage compositeToPoint:drawRect.origin operation:NSCompositeSourceOver ];
    }
}

-(NSRect*)boundingRect
{
    return &drawRect;
}

-(NSNumber*)idNo
{
    return idno;
}

-(NSString*)description
{
    return [ NSString stringWithFormat:@"ID=%@,Rect=%@,selectedImage=%@",
        idno,NSStringFromRect(drawRect),selectedImage ];
}

- (void)dealloc
{
    [ idno release ];
    [ selectedImage release ];

    [super dealloc];
}

@end

@implementation TYSpriteButton
-(id)initWithID:(NSNumber*)no spriteNo:(NSNumber*)sNo appendAction:(NSString*)string
{
    self = [ super init ];
    
    idno = [ no retain ];
    spriteNo = [ sNo retain ];
    appendAction = [ string retain ];

    return self;
}

-(NSNumber*)idNo
{
    return idno;
}

-(int)spriteNo
{
    return [ spriteNo intValue ];
}

-(NSString*)appendString
{
    return appendAction;
}

- (void)dealloc
{
	[idno release];
        [spriteNo release];
        [appendAction release];
	[super dealloc];
}

@end

@implementation TYCselButton

-(id)initWithID:(NSNumber*)no selctionNo:(int)sNo image:(TYCellImage*)image point:(NSPoint)aPoint
{
    [ super init ];
    
    idno = [ no retain ];
    selNo = sNo;
    strImage = [ image retain ];
    drawPoint = aPoint;
    
    return self;
}

-(void)drawWithSelect:(BOOL)aBool
{
    [ strImage changeCell:aBool ];
    [ (NSImage*)strImage compositeToPoint:drawPoint operation:NSCompositeSourceOver ];
}

-(NSNumber*)idNo { return idno; }
-(int)cselNo { return selNo; }

- (void)dealloc
{
	[idno release];
        [strImage release ];
	[super dealloc];
}

@end
