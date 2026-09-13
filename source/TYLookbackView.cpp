//
//  TYLookbackView.m
//  Tukuyomi
//
//  Created by toveta on Thu Mar 06 2003.
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

#import "TYLookbackView.h"
#import "TYLookbackLayer.h"
#import "TYMainController.h"
#import "TYStageManager.h"
#import "TYKeyBind.h"
#import "TYEnviroment.h"
#import "TYCellImage.h"
#import "TYControllButton.h"

@implementation TYLookbackView

static id manager;

-(id)initWithFrame:(NSRect)aFrame
        controller:(id)aObj
        attributes:(NSDictionary*)aDict
            layout:(NSMutableDictionary*)layoutDic
            buffer:(NSArray*)aArray
           ubutton:(TYCellImage*)uImage
           dbutton:(TYCellImage*)dImage
{
    self = [ super initWithFrame:aFrame ];

    manager = [ TYStageManager sharedManager ];
        
    controller = aObj;
    
    //menuWindowColor = [ [ layoutDic objectForKey:TYRMenuBGColor ] retain ];

    attDict = [ aDict retain ];
    
    lookbackArray = [ aArray retain ];

    //drawBuffer = [ [ NSImage alloc ] initWithSize:TYVirtualScreenSize() ];
    drawBuffer = [ [ manager visualLayer ] copy ];
    [ drawBuffer lockFocus ];
    [ manager drawTextWindow ];
    [ drawBuffer unlockFocus ];
    
    // ボタン配置
    button[0] = nil;
    button[1] = nil;
    {
        NSRect twrect;
        
        twrect = [ manager textWindowRect ];
        
        if(uImage) {
            btnRect[0].size = [ (NSImage*)uImage size ];
            btnRect[0].origin.x = NSMaxX(twrect) -btnRect[0].size.width;
            btnRect[0].origin.y = NSMaxY(twrect) -btnRect[0].size.height;
            button[0] = [ [ TYControllButton alloc ] initWithFrame:btnRect[0]
                                                        cellImage:uImage ];
            [ self addSubview:button[0] ];
            [ button[0] setTarget:self ];
            [ button[0] setAction:@selector(execButton:) ];
        }

        if(dImage) {
            btnRect[1].size = [ (NSImage*)dImage size ];
            btnRect[1].origin.x = NSMaxX(twrect) -btnRect[1].size.width;
            btnRect[1].origin.y = NSMinY(twrect);
            button[1] = [ [ TYControllButton alloc ] initWithFrame:btnRect[1]
                                                        cellImage:dImage ];
            [ self addSubview:button[1] ];
            [ button[1] setTarget:self ];
            [ button[1] setAction:@selector(execButton:) ];
        }
    }
    
    
    [ self setPageIndex:[ lookbackArray count ] -1 ];

    return self;
}

-(void)setPageIndex:(int)aInt
{
    index = aInt;
    NSArray *voiceArray;
    
    [ [ lookbackArray objectAtIndex:index ] setManager:manager ];
    [ [ lookbackArray objectAtIndex:index ] setAttribute:attDict ];
    //[ [ lookbackArray objectAtIndex:index ] recache ];
    
    // ボイスのリプレイ
    if([ TYEnviroment boolForKey:TYLookbackVoiceReplayEnviroment ]){
        if(voiceArray = [ [ lookbackArray objectAtIndex:index ] voiceArray ]){
            [ controller playWaveChannel:DIRECT_WAVE_REPLAY
                                    path:[ [ voiceArray objectAtIndex:0 ] objectForKey:TYPathVoiceLookbackData ] 
                                    loop:NO ];
        }
    }
    
    // ボタンの表示、非表示
    {
        NSRect frame;
        int cnt = [ lookbackArray count ];
        
        frame = [ button[0] frame ];
        frame.size = (index == 0) ? NSZeroSize : btnRect[0].size ;
        [ button[0] setFrame:frame ];

        frame = [ button[1] frame ];
        frame.size = (index == cnt -1) ? NSZeroSize : btnRect[1].size ;
        [ button[1] setFrame:frame ];
    }
}

-(void)updatesTrackingRect
{
    if(button[0])
        rectTag[0] = [ self addTrackingRect:btnRect[0] owner:self userData:nil assumeInside:NO ];
        
    if(button[1])
        rectTag[1] = [ self addTrackingRect:btnRect[1] owner:self userData:nil assumeInside:NO ];
}

-(void)removeAllTrackingRect
{
    if(rectTag[0])
        [ self removeTrackingRect:rectTag[0] ];
    
    if(rectTag[1])
        [ self removeTrackingRect:rectTag[1] ];
}

- (void)drawRect:(NSRect)rect {
    [ drawBuffer compositeToPoint:rect.origin
                         fromRect:rect
                        operation:NSCompositeCopy ];
    if(NSEqualRects(rect,TYVirtualScreenRect()))
        [ [ lookbackArray objectAtIndex:index ] drawImage ];
}

-(BOOL)isOpaque
{
    return YES;
}

-(IBAction)execButton:(id)sender
{
    if(sender == button[0])
        [ self up_action ];
    else
        [ self down_action ];
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

-(void)mouseEntered:(NSEvent*)theEvent
{
    if(rectTag[0] == [ theEvent trackingNumber ])
        [ button[0] changeCell:1 ];
    else
        [ button[1] changeCell:1 ];
}

-(void)mouseExited:(NSEvent*)theEvent
{
    if(rectTag[0] == [ theEvent trackingNumber ])
        [ button[0] changeCell:0 ];
    else
        [ button[1] changeCell:0 ];
}

-(void)mouseMoved:(NSEvent*)theEvent
{
    ;
}

-(void)mouseDown:(NSEvent*)theEvent
{
    ;
}

-(void)rightMouseDown:(NSEvent*)theEvent
{
    [ self cancel_action:nil ];
}

-(void)keyDown:(NSEvent*)theEvent
{
    [ self performSelector:TYSelectorForKeycode([ theEvent keyCode ]) withObject:nil ];
}

-(void)cancel_action:(id)sender
{
    [ self endMode ];
}

-(void)down_action
{
    if(index +1 >= [ lookbackArray count ]){
        [ self endMode ];
        return;
    }

    [ self setPageIndex:index +1 ];
    [ self setNeedsDisplay:YES ];
}

-(void)up_action
{
    if(index -1 < 0)
        return;
        
    [ self setPageIndex:index -1 ];
    [ self setNeedsDisplay:YES ];    
}

- (void)scrollWheel:(NSEvent *)theEvent
{
    if([ theEvent deltaY ] > 0.0){
        [ self up_action ];
    } else {
        [ self down_action ];
    }
}

-(void)endMode
{
    [ self removeAllTrackingRect ];
    [ [ self window ] makeFirstResponder:[ self superview ] ];
    [ controller exitSystemModeResumeStatus:YES ];
    [ [ self retain ] autorelease ];
    [ self removeFromSuperview ];    
}

// delegate methods
-(void)drawTextWindow { /* do nothing */ ; }
-(void)drawAttStr:(id)str
         atColumn:(int)col
            atRow:(int)row
{
    [ manager drawAttStr:str atColumn:col atRow:row ];
}
// end delegate methods

- (void)dealloc
{
        [ drawBuffer release ];
	//[menuWindowColor release];
        [attDict release];
        [lookbackArray release];

        [ button[0] release ];
        [ button[1] release ];
	[super dealloc];
        
}

@end
