//
//  TYVisualNovelView.m
//  Tukuyomi
//
//  Created by toveta on Tue Sep 25 2001.
//  Copyright (c) 2001 toveta. All rights reserved.
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

#import "TYVisualNovelView.h"
#import "TYEnviroment.h"
#import "TYKeyBind.h"

#define POINT_DIV(p,s) p.x/=s;p.y/=s;
#define RECT_DIV(r,s) r.origin.x/=s;r.origin.y/=s;r.size.width/=s;r.size.height/=s;
#define RECT_MUL(r,s) r.origin.x*=s;r.origin.y*=s;r.size.width*=s;r.size.height*=s;

@implementation TYVisualNovelView

/*
-(void)setMouseDownSelector:(SEL)aSelector
{
    TYMouseDownSelector = aSelector;
}
*/

/*
- (id)initWithFrame:(NSRect)frameRect
{    
    [ super initWithFrame:frameRect ];
    return self;
}
*/

- (void)drawRect:(NSRect)rect
{
    if(directDraw){
        if(NSEqualRects(rect,directInRect)){
            [ directDrawImage drawInRect:directInRect fromRect:directFromRect operation:NSCompositeCopy fraction:1.0 ];
            directDraw=NO;
            [ directDrawImage release ];
            return;
        }
    }

    if(isScale){
        if(sourceImage){
            NSRect fRect=rect;
            RECT_DIV(fRect,scale);
            [ sourceImage drawInRect:rect fromRect:fRect operation:NSCompositeCopy fraction:1.0 ];
            //[ sourceImage compositeToPoint:rect.origin fromRect:rect operation:NSCompositeCopy ];
        } else {
            [ [ NSColor blackColor ] set ];
            NSRectFill(rect);
        }
    } else {
        if(sourceImage){
            [ sourceImage compositeToPoint:rect.origin fromRect:rect operation:NSCompositeCopy ];
        } else {
            [ [ NSColor blackColor ] set ];
            NSRectFill(rect);
        }
    }

    if(directDraw){
        [ directDrawImage drawInRect:directInRect fromRect:directFromRect operation:NSCompositeCopy fraction:1.0 ];
        directDraw=NO;
        [ directDrawImage release ];
    }
    
}

- (BOOL)isOpaque
{
    return YES;
}

/*
- (BOOL)isFlipped
{
    return YES;
}
*/

-(NSImage*)image
{
    return sourceImage;
}

-(void)setImage:(NSImage*)image
{
    [ sourceImage autorelease ];
    sourceImage = image;
    [ sourceImage retain ];
}

-(void)directDrawImage:(NSImage*)image inRect:(NSRect)inRect fromRect:(NSRect)fromRect
{
    /*
    [ self lockFocus ];
    [ image drawInRect:inRect fromRect:fromRect operation:NSCompositeCopy fraction:1.0 ];
    [ self unlockFocus ];

    [ [ NSGraphicsContext graphicsContextWithWindow:[ self window ] ] flushGraphics ];
    return ;
     */
    
    directDraw=YES;
    directDrawImage = [ image retain ];
    if(isScale){
        RECT_MUL(inRect,scale);
    }
    directInRect = inRect;
    directFromRect = fromRect;
    [ self setNeedsDisplayInRect:directInRect ];
    //[ self display ];
}

-(void)setTarget:(id)aTarget
{
    target = aTarget;
}

-(void)setScaling:(float)s
{
    isScale = (s != 1.0) ? YES : NO;

    scale = s;
}
-(BOOL)isScaling { return isScale; }
-(float)scale { return scale; }

-(void)setNeedsDisplayScalingRect:(NSRect)sRect
{
    if(isScale){
        RECT_MUL(sRect,scale);
    }
    [ super setNeedsDisplayInRect:sRect ];
}

-(void)mouseDown:(NSEvent *)theEvent
{
    if([ theEvent modifierFlags ] & NSControlKeyMask){
        [ target cancel_action:self ];
    } else {
        [ target select_action ];
    }
}

-(void)rightMouseDown:(NSEvent *)theEvent
{
    [ target cancel_action:self ];
}

-(void)keyDown:(NSEvent *)theEvent
{    
    [ NSCursor setHiddenUntilMouseMoves:YES ];
    [ target performSelector:TYSelectorForKeycode([ theEvent keyCode ]) withObject:nil ];
}
- (void)mouseMoved:(NSEvent *)theEvent
{
    if(isScale){
        NSPoint point=[ self convertPoint:[ theEvent locationInWindow ] fromView:nil ];
        POINT_DIV(point,scale);
        [ target move_mouse:point ];
    } else {
        [ target move_mouse:[ self convertPoint:[ theEvent locationInWindow ] fromView:nil ] ];
    }
}

- (void)scrollWheel:(NSEvent *)theEvent
{
    if([ theEvent deltaY ] > 0.0){
        [ target up_action ];
    } else {
        [ target down_action ];
    }
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

@end
