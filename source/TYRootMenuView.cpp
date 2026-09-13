//
//  TYRootMenuView.m
//  Tukuyomi
//
//  Created by toveta on Sun Mar 02 2003.
//  Copyright (c) 2003 toveta. All rights reserved.
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
 
#import "TYRootMenuView.h"
#import "TYStringImage.h"
#import "TYCellImage.h"
#import "TYStageManager.h"
#import "TYControllButton.h"


@implementation TYRootMenuView
- (id)initWithFrame:(NSRect)aFrame
        controller:(id)aObj
        attributes:(NSDictionary*)aDict
        menuItems:(NSArray*)aArray
        layout:(NSMutableDictionary*)layoutDict
{
    int fontWidth;
    int fontHeight;
    int pitchX;
    int pitchY;
    BOOL shadow;
    
    int y,cnt,tag;
    NSEnumerator *enu;
    NSString* temp;
    TYCellImage *image;
    NSRect frame;
    NSSize size;
    TYControllButton *button;
    NSArray *buttonColors;
    NSMutableArray *buttonArray;
    NSMutableArray *actionArray;
    NSColor *menuColor;
    
    buttonArray = [ NSMutableArray array ];
    actionArray = [ NSMutableArray array ];

	// 初期化
    self = [super initWithFrame:aFrame ];

    if (!self)
        return nil;
	
    // メニューレイアウトの読み込み
    {
        size = [ [ layoutDict objectForKey:TYRMenuFontSize ] sizeValue ];
        fontWidth = size.width;
        fontHeight = size.height;
        
        size = [ [ layoutDict objectForKey:TYRMenuFontPitchSize ] sizeValue ];
        pitchX = size.width;
        pitchY = size.height;
        
        shadow = [ [ layoutDict objectForKey:TYRMenuShadowed ] boolValue ];
        
        menuColor = [ layoutDict objectForKey:TYRMenuBGColor ];
        
        buttonColors = [ NSArray arrayWithObjects:
                        [ layoutDict objectForKey:TYRMenuNoSelectColor ],
                        [ layoutDict objectForKey:TYRMenuSelectColor ],nil ];
    }

    cnt = [ aArray count ] / 2;
    enu = [ aArray objectEnumerator ];
    
    // 1行目の位置取得。
    y = VSCREEN_HEIGHT -(VSCREEN_HEIGHT + (-fontHeight -pitchY) *(cnt -1)) /2;

    tag = 0;
    while(temp = [ enu nextObject ]) {
        NSPoint origin;
        
        // イメージ生成
        image = [ [ TYCellImage alloc ] initWithImages:
                    [ TYStringImage imagesWithString:temp 
                                            attributes:aDict 
                                                colors:buttonColors
                                            fontHeight:fontHeight
                                            fontWidth:fontWidth 
                                            interval:pitchX 
                                                shadow:shadow ] ];
        [ image autorelease ];
        
        // button生成。
        size = [ (NSImage*)image size ];
        frame = NSMakeRect((VSCREEN_WIDTH -size.width) / 2,
                            y,size.width,size.height);
        origin = [ (TYStringImage*)image origin ];
        frame.origin.x += origin.x;
        frame.origin.y += origin.y;
        [ (NSArray*)image makeObjectsPerformSelector:@selector(setOriginByValue:)
                withObject:[ NSValue valueWithPoint:NSZeroPoint ] ];

        
        button = [ [ [ TYControllButton alloc ] initWithFrame:frame cellImage:image ] autorelease ];
        [ button setTarget:self ];
        [ button setAction:@selector(execButton:) ];
        [ button setTag:tag++ ];

		// TODO:初期化されてない自身に対する呼び出し。このメソッドをinitWithFrame:とsetController:〜に分割。
        [ self addSubview:button ];
        [ buttonArray addObject:button ];
        
        [ actionArray addObject:[ enu nextObject ] ];
        y -= fontHeight +pitchY;
    }
    
    // 初期化完了
    [self setController:aObj
			buttonArray:buttonArray
			actionArray:actionArray
				bgColor:menuColor ];

    return self;
}

@end
