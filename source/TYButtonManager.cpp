//
//  TYButtonManager.m
//  Tukuyomi
//
//  Created by toveta on Thu Oct 11 2001.
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

#import "TYButtonManager.h"
#import "TYStageManager.h"

@implementation TYButtonManager
-(id)initWithImage:(NSImage*)srcImage
{
    self = [ super init ];
    
    // イメージの保持
    sourceImage = srcImage;
    [ sourceImage retain ];
    
    // インスタンスの確保
    btnArray = [ [ NSMutableArray alloc ] initWithCapacity:1 ];

    drawBuffer = [ [ NSImage alloc ] initWithSize:TYVirtualScreenSize() ];
    
    beforeSelection = TYButtonUnInitialize;
    
    rectMap = [ [ NSMutableData dataWithLength:VSCREEN_WIDTH *VSCREEN_HEIGHT ] retain ];
    
    return self;
}

-(NSImage*)image
{
    return sourceImage;
}

-(void)addButton:(NSNumber*)no x:(NSNumber*)x y:(NSNumber*)y width:(NSNumber*)width height:(NSNumber*)height selx:(NSNumber*)selx sely:(NSNumber*)sely
{
    NSImage *unselImage,*selImage;
    int w,h,sx,sy,i,j;
    NSSize srcSize;
    NSRect rect;
    unsigned char *map,*ptr;
    int index;
    
    w = [ width intValue ];
    h = [ height intValue ];
    sx = [ selx intValue ];
    sy = [ sely intValue ];
    rect = NSMakeRect([ x intValue ],VSCREEN_HEIGHT -[ y intValue ] -h,w,h);
    
    // 未選択状態のイメージを作成
    unselImage = [ [ [ NSImage alloc ] initWithSize:NSMakeSize(w,h) ] autorelease ];
    
    // 選択状態のイメージを作成
    selImage = [ [ [ NSImage alloc ] initWithSize:NSMakeSize(w,h) ] autorelease ];
    srcSize = [ sourceImage size ];
    [ selImage lockFocus ];
    [ sourceImage compositeToPoint:NSZeroPoint fromRect:NSMakeRect(sx,srcSize.height -h -sy,w,h) operation:NSCompositeSourceOver ];

    [ selImage unlockFocus ];

    // ボタン定義の追加
    [ btnArray addObject:[ [ [ TYButton alloc ] initWithID:no image:unselImage selectedImage:selImage rect:rect ] autorelease ] ];

    // マウス反応用のビットマップ定義
    map = (unsigned char*)[ rectMap mutableBytes ];
    index = [ btnArray count ];
    for(i = 0; i < rect.size.height; i++){
        ptr = map +(int)rect.origin.x +( (int)rect.origin.y +i) *VSCREEN_WIDTH; 
        for(j = 0; j < rect.size.width; j++,ptr++){
            *ptr = index;
        }
    }
}

-(NSData*)rectMap
{
    return rectMap;
}

-(void)initialButtonImage
{
    // ボタンモード開始前の、ボタン画像の初期化
    NSEnumerator *enm;
    id temp;
    
    // 初期化されていなければ、全てのボタンを描く
    if(beforeSelection==TYButtonUnInitialize){
        enm = [ btnArray objectEnumerator ];
        while( (temp = [ enm nextObject ]) != nil){
            [ temp drawWithSelect:NO ];
        }
        beforeSelection = TYButtonNoSelected;
        return;
    }
    
}

-(int)selectedButtonID
{
    if(beforeSelection) {
        return [ [ [ btnArray objectAtIndex:(beforeSelection -1) ] idNo ] intValue ];
    } else {
        return 0;
    }
}

-(int)incToIndex
{
    int nowSelection,count;
    
    count = [ btnArray count ];

    if(!count){
        return 0;
    }
    
    nowSelection = beforeSelection +1;
    if(nowSelection > count ){
        return 1;
    } else {
        return nowSelection;
    }
    
}

-(int)decToIndex
{
    int nowSelection,count;
    
    count = [ btnArray count ];

    if(!count){
        return 0;
    }

    nowSelection = beforeSelection -1;
    if(nowSelection <= 0){
        return count;
    } else {
        return nowSelection;
    }
}

-(void)drawWithSelection:(int)status
{
    [ drawBuffer lockFocus ];
    {
        // 前回選択状態だったボタンを非選択状態で描く
        if(beforeSelection){
            [ [ btnArray objectAtIndex:(beforeSelection -1) ] drawWithSelect:NO ];
        }
        
        // 今回選択されたボタンを選択状態で描く
        if(status){
            [ [ btnArray objectAtIndex:(status -1) ] drawWithSelect:YES ];
        }
    }
    [ drawBuffer unlockFocus ];
    
    [ drawBuffer compositeToPoint:NSZeroPoint operation:NSCompositeSourceOver ];
    beforeSelection = status;
}

-(NSString*)description
{
    return [ btnArray description ];
}

- (void)dealloc
{
    [ sourceImage release ];
    [ btnArray release ];
    [ drawBuffer release ];
    [ rectMap release ];

    [super dealloc];
}

@end

@implementation TYButton
-(id)initWithID:(NSNumber*)no image:(NSImage*)unselImage selectedImage:selImage rect:(NSRect)aRect
{
    self = [ super init ];
    
    idno = [ no retain ];
    unselectedImage = unselImage;
    [ unselectedImage retain ];
    selectedImage = selImage;
    [ selectedImage retain ];
    drawRect = aRect;
    
    return self;
}

-(void)drawWithSelect:(BOOL)aBool
{
    NSImage *drawImage;
    
    // 自身のRectをクリアしてから描画
    [ [ NSColor blackColor ] set ];
    NSRectFillUsingOperation(drawRect,NSCompositeClear);
    /*
    [ [ NSColor clearColor ] set ];
    [ NSBezierPath fillRect:drawRect ];
    */
    
    drawImage = (aBool) ? selectedImage : unselectedImage ;
    [ drawImage compositeToPoint:drawRect.origin operation:NSCompositeSourceOver ];
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
    return [ NSString stringWithFormat:@"ID=%@,Rect=%@,unselImage=%@,selectedImage=%@",
                    idno,NSStringFromRect(drawRect),unselectedImage,selectedImage ];
}

- (void)dealloc
{
    [ idno release ];
    [ unselectedImage release ];
    [ selectedImage release ];
    
    [super dealloc];
}

-(id)copyWithZone:(NSZone*)zone
{
    id tempcopy = [ [ [ self class ] allocWithZone:zone ] init ];
    
    // 浅いコピーでいいかな？
    tempcopy->idno = [ idno retain ];
    tempcopy->unselectedImage = [ unselectedImage retain ];
    tempcopy->selectedImage = [ selectedImage retain ];
    tempcopy->drawRect = drawRect;
    
    return tempcopy;
}

-(BOOL)isOverlay
{
    return NO;
}

@end
