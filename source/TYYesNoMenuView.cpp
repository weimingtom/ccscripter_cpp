//
//  TYYesNoMenuView.m
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
 
#import "TYYesNoMenuView.h"
#import "TYStringImage.h"
#import "TYCellImage.h"
#import "TYStageManager.h"
#import "TYControllButton.h"


@implementation TYYesNoMenuView
-(id)initWithFrame:(NSRect)aFrame
        controller:(id)aObj
        attributes:(NSDictionary*)aDict
            titleAndAction:(NSArray*)aArray
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

    cnt = 2;
    
    // 1行目の位置取得。
    y = VSCREEN_HEIGHT -(VSCREEN_HEIGHT + (-fontHeight -pitchY) *(cnt -1)) /2;

    tag = 0;
    
    enu = [ aArray objectEnumerator ];
    while(temp = [ enu nextObject ]){
        NSPoint origin;
        NSString *aStr;
        
        switch (tag) {
        case 0:
            aStr = [ aArray objectAtIndex:0 ];
            break;
        case 1:
            aStr = NSLocalizedString(@"YES",nil);
            break;
        case 2:
            aStr = NSLocalizedString(@"NO",nil);
            break;
        }
        
        // イメージ生成
        image = [ [ TYCellImage alloc ] initWithImages:
                    [ TYStringImage imagesWithString:aStr 
                                            attributes:aDict 
                                                colors:(tag == 0) ? [ NSArray arrayWithObject:[ layoutDict objectForKey:TYRMenuSelectColor ] ] : buttonColor
                                            fontHeight:fontHeight
                                            fontWidth:fontWidth 
                                            interval:pitchX 
                                                shadow:shadow ] ];
        [ image autorelease ];
        
        // button生成。
        size = [ (NSImage*)image size ];
        if(tag == 0) {
            frame = NSMakeRect((VSCREEN_WIDTH -size.width) / 2,
                                y,size.width,size.height);
        } else {
            int x;
            x = VSCREEN_WIDTH /2 + size.width *((tag == 1) ? -1.3 : 0.3);
            frame = NSMakeRect(x,y,size.width,size.height);
        }
        origin = [ (TYStringImage*)image origin ];
        frame.origin.x += origin.x;
        frame.origin.y += origin.y;
        [ (NSArray*)image makeObjectsPerformSelector:@selector(setOriginByValue:)
                withObject:[ NSValue valueWithPoint:NSZeroPoint ] ];
            
        button = [ [ [ TYControllButton alloc ] initWithFrame:frame cellImage:(tag == 0) ? [ (NSArray*)image objectAtIndex:0 ] : image ] autorelease ];
        [ self addSubview:button ];

        if(tag != 0){
            [ button setTarget:self ];
            [ button setAction:@selector(execButton:) ];
            [ button setTag:tag -1 ];
            [ buttonArray addObject:button ];
            [ actionArray addObject:temp ];
        }
        
        if(tag == 0)
            y -= fontHeight +pitchY;
            
        tag++;
    }
    
    // 初期化完了
    [self setController:aObj
			buttonArray:buttonArray
			actionArray:actionArray
				bgColor:menuColor ];
	
    return self;
}

/*
-(void)mouseEntered:(NSEvent*)theEvent
{
    [ super mouseEntered:theEvent ];
}
*/

@end
