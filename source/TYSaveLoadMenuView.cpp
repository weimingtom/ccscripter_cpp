//
//  TYSaveLoadMenuView.m
//  Tukuyomi
//
//  Created by toveta on Sun Mar 02 2003.
//  Copyright (c) 2003 toveta All rights reserved.
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

#import "TYSaveLoadMenuView.h"
#import "TYStringImage.h"
#import "TYCellImage.h"
#import "TYStageManager.h"
#import "TYControllButton.h"

@implementation TYSaveLoadMenuView
-(id)initWithFrame:(NSRect)aFrame
        controller:(id)aObj
            layout:(NSMutableDictionary*)layoutDict
        attributes:aDict
      dateStrArray:(NSArray*)dArray
       existsArray:(NSArray*)eArray
            doLoad:(BOOL)doLoad
{
    int fontWidth;
    int fontHeight;
    int pitchX;
    int pitchY;
    BOOL shadow;
    
    int y,cnt,tag,idx;
    NSEnumerator *denu,*eenu;
    NSString* temp;
    TYCellImage *image;
    NSRect frame;
    NSSize size;
    TYControllButton *button;
    NSArray *buttonColor;
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
        
        buttonColor = [ NSArray arrayWithObjects:
                        [ layoutDict objectForKey:TYRMenuNoSelectColor ],
                        [ layoutDict objectForKey:TYRMenuSelectColor ],nil ];
    }

    cnt = [ dArray count ];
    denu = [ dArray objectEnumerator ];
    eenu = [ eArray objectEnumerator ];
    
    // 1行目の位置取得。
    y = VSCREEN_HEIGHT -(VSCREEN_HEIGHT + (-fontHeight -pitchY) *(cnt -1)) /2;

    tag = 0;
    idx = 0;
    while(temp = [ denu nextObject ]) {
        NSPoint origin;
        NSArray *color;
        NSNumber *existsNum;
        
        if(idx != 0)
            existsNum = [ eenu nextObject ];
        
        if(idx == 0)
            color = [ NSArray arrayWithObject:[ layoutDict objectForKey:TYRMenuSelectColor ] ];
        else if(doLoad && ![ existsNum boolValue ])
            color = [ NSArray arrayWithObject:[ layoutDict objectForKey:TYRMenuNoDataColor ] ];
        else
            color = buttonColor;
        
        // イメージ生成
        image = [ [ TYCellImage alloc ] initWithImages:
                    [ TYStringImage imagesWithString:temp 
                                            attributes:aDict 
                                                colors:color
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
        if(idx != 0 && (!doLoad || [ existsNum boolValue ])){            
            [ button setTarget:self ];
            [ button setAction:@selector(execButton:) ];
            [ button setTag:tag++ ];
            [ buttonArray addObject:button ];
            [ actionArray addObject:[ NSString stringWithFormat:
                                        (doLoad) ? @"_qload%d" : @"_qsave%d" ,idx ] ];
        }

        [ self addSubview:button ];
        
        y -= fontHeight +pitchY;
        idx++;
    }
    
    // 初期化完了
    [self setController:aObj
			buttonArray:buttonArray
			actionArray:actionArray
				bgColor:menuColor ];
	
    return self;
}


@end
