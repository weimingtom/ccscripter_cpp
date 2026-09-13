//
//  TYRespondRectManager.m
//  Tukuyomi
//
//  Created by toveta on Tue Nov 06 2001.
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

#import "TYRespondRectManager.h"

@implementation TYRespondRectManager
-(id)initWithStageManager:(TYStageManager*)manager
{
    self = [ super init ];
    
    stageManager = [ manager retain ];
    
    // インスタンスの確保
    drawBuffer = [ [ NSImage alloc ] initWithSize:TYVirtualScreenSize() ];

    btnArray = [ [ NSMutableArray alloc ] initWithCapacity:1 ];
    
    beforeSelection = -1;
    
    rectMap = [ [ NSMutableData dataWithLength:VSCREEN_WIDTH *VSCREEN_HEIGHT ] retain ];

    return self;
}

-(void)addSelection:(id)aButton rect:(NSRect)rect
{
    unsigned char *map,*ptr;
    int index,i,j;

    // cellImageを配列に突っ込む。
    [ btnArray addObject:aButton ];

    // rectMapに登録
    map = (unsigned char*)[ rectMap mutableBytes ];
    index = [ btnArray count ];
    // rect が画面サイズを超えないように
    rect = NSIntersectionRect(rect,TYVirtualScreenRect());
    for(i = 0; i < rect.size.height; i++){
        ptr = map +(int)rect.origin.x +( (int)rect.origin.y +i) *VSCREEN_WIDTH;
        for(j = 0; j < rect.size.width; j++,ptr++){
            *ptr = index;
        }
    }

}

-(void)initialButtonImage
{
    return;
}

-(int)selectedButtonID
{
    return beforeSelection; // デフォルトではindex=idNo;
}

-(int)selectedIndex
{
    return beforeSelection; // 常にindexを返す。
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

-(NSData*)rectMap
{
    return rectMap;
}

-(BOOL)changeSelection:(int)index
{
    if(beforeSelection == index)
        return NO;
    
    beforeSelection = index;
    
    return YES;
}

-(void)draw
{
    [ drawBuffer compositeToPoint:NSZeroPoint operation:NSCompositeSourceOver ];
}

-(void)clear
{
    // インスタンスの再確保またはクリアー
    if(drawBuffer)
        [ drawBuffer release ];
    drawBuffer = [ [ NSImage alloc ] initWithSize:TYVirtualScreenSize() ];

    if(btnArray)
        [ btnArray release ];
    btnArray = [ [ NSMutableArray alloc ] initWithCapacity:1 ];
    
    beforeSelection = -1;

    if(rectMap) {
        memset([ rectMap mutableBytes ],0,(size_t)VSCREEN_WIDTH * VSCREEN_HEIGHT);
    }

}

- (void)dealloc
{
        if(stageManager){
            [ stageManager release ];
        }
        [drawBuffer release ];
	[btnArray release];
	[rectMap release ]; 
        [super dealloc];
}

@end
