//
//  TYMenuModeView.m
//  Tukuyomi
//
//  Created by toveta on Thu Feb 06 2003.
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

#import "TYMenuModeView.h"
#import "TYControllButton.h"
#import "TYKeyBind.h"
#import "TYMainController.h"
#import "TYVisualNovelView.h"

@implementation TYMenuModeView

-(id)initWithFrame:(NSRect)aFrame
{
    // 初期化
    self = [super initWithFrame:aFrame];
    if (!self)
        return nil;
	
	return self;
}

-(void) setController:(id)aObj
        buttonArray:(id)bArray
        actionArray:(id)aArray
        bgColor:(NSColor*)aColor
{
    controller = aObj;
    
    selectedButton = nil;

    btnArray = [ bArray retain ];
    
    actArray = [ aArray retain ];

    menuWindowColor = [ aColor retain ];        
}

-(IBAction)execButton:(id)sender
{
    [ self select_action ];
}

- (void)drawRect:(NSRect)rect {
    // Drawing code here.
    [ menuWindowColor set ];
    NSRectFillUsingOperation(rect,NSCompositeSourceOver);
}

-(void)updatesTrackingRect
{
    NSEnumerator *enu;
    TYControllButton *temp;
    NSTrackingRectTag tag;

    if(btnDict)
        [ self removeAllTrackingRect ];

    btnDict = [ [ NSMutableDictionary dictionary ] retain ];
    enu = [ btnArray objectEnumerator ];
    while(temp = [ enu nextObject ]){
        tag = [ self addTrackingRect:[ temp frame ] owner:self userData:nil assumeInside:NO ];
        [ btnDict setObject:temp forKey:[ NSNumber numberWithInt:tag ] ];
    }
}

-(void)removeAllTrackingRect
{
    NSEnumerator *enu;
    NSNumber *temp;
    
    if(!btnDict)
        return;

    if(selectedButton != nil) {
        [ selectedButton changeCell:0 ];
        [ selectedButton setNeedsDisplay:YES ];
    }
        
    selectedButton = nil;

    enu = [ btnDict keyEnumerator ];
    while(temp = [ enu nextObject ]){
        [ self removeTrackingRect:[ temp intValue ] ];
    }
    
    [ btnDict release ];
    btnDict = nil;
}

-(void)setButtonArray:(NSArray*)aArray
{
    if(btnArray)
        [ btnArray autorelease ];
    btnArray = [ aArray retain ];
}

-(void)setActionArray:(NSArray*)aArray
{
    if(actArray)
        [ actArray autorelease ];
    actArray = [ aArray retain ];
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

-(void)mouseEntered:(NSEvent*)theEvent
{
    TYControllButton *btn;
    int no;

    no = [ theEvent trackingNumber ];
    btn = [ btnDict objectForKey:[ NSNumber numberWithInt:no ] ];
    [ btn changeCell:1 ];
    [ btn setNeedsDisplay:YES ];
        
    if(selectedButton && (btn != selectedButton)){
        [ selectedButton changeCell:0 ];
    }
    
    selectedButton = btn;
}

-(void)mouseExited:(NSEvent*)theEvent
{
    TYControllButton *btn;
    int no;
    
    no = [ theEvent trackingNumber ];
    btn = [ btnDict objectForKey:[ NSNumber numberWithInt:no ] ];
    [ btn changeCell:0 ];
    [ btn setNeedsDisplay:YES ];
    
    if(selectedButton == btn)
        selectedButton = nil;
}

-(void)mouseMoved:(NSEvent*)theEvent
{
    ;
}

-(void)mouseDown:(NSEvent*)theEvent
{
    ;
}

-(void)keyDown:(NSEvent*)theEvent
{
    [ self performSelector:TYSelectorForKeycode([ theEvent keyCode ]) withObject:nil ];
}

- (void)scrollWheel:(NSEvent *)theEvent
{
    if([ theEvent deltaY ] > 0.0){
        [ self up_action ];
    } else {
        [ self down_action ];
    }
}


-(void)down_action
{
    TYControllButton *btn;
    int idx;
    
    idx = (selectedButton == nil) ? -1 : [ btnArray indexOfObject:selectedButton ];

    if((idx == [ btnArray count ] -1) || idx == -1){
        btn = [ btnArray objectAtIndex:0 ];
    } else {
        btn = [ btnArray objectAtIndex:idx +1 ];
    }
    
    if(selectedButton != btn){
        [ selectedButton changeCell:0 ];
        [ selectedButton setNeedsDisplay:YES ];
        [ btn changeCell:1 ];
        [ btn setNeedsDisplay:YES ];
        selectedButton = btn;
    }    
}

-(void)rightMouseDown:(NSEvent*)theEvent
{
    [ self cancel_action:nil ];
}

-(void)select_action
{
    int idx;
    
    if(!selectedButton)
        return;
    
    idx = [ btnArray indexOfObject:selectedButton ];
    [ self endMode ];
    [ controller ty_systemcall:[ NSMutableArray arrayWithObjects:@"systemcall",[ actArray objectAtIndex:idx ] ,nil ] ];
}

-(void)cancel_action:(id)sender
{
    /*
    [ controller exitSystemModeResumeStatus:YES ];
    [ self removeFromSuperview ];    
     */
    [ self endMode ];
}

-(void)up_action
{
    TYControllButton *btn;
    int idx;
    
    idx = (selectedButton == nil) ? -1 : [ btnArray indexOfObject:selectedButton ];

    if(idx == 0 || idx == -1){
        btn = [ btnArray lastObject ];
    } else {
        btn = [ btnArray objectAtIndex:idx -1 ];
    }
    
    if(selectedButton != btn){
        [ selectedButton changeCell:0 ];
        [ selectedButton setNeedsDisplay:YES ];
        [ btn changeCell:1 ];
        [ btn setNeedsDisplay:YES ];
        selectedButton = btn;
    }
}

-(void)endMode
{
    [ [ self window ] makeFirstResponder:[ self superview ] ];
    [ self removeAllTrackingRect ];
    [ controller exitSystemModeResumeStatus:YES ];
    [ [ self retain ] autorelease ];
    [ self removeFromSuperview ];    
}

- (void)dealloc
{
	[menuWindowColor release];
        [btnDict release];
        [btnArray release];
        [actArray release];
	[super dealloc];
}

@end
