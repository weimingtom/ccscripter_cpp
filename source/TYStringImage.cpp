//
//  TYStringImage.m
//  Tukuyomi
//
//  Created by toveta on Wed May 08 2002.
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

#import "TYStringImage.h"
#import "TYStageManager.h"


@implementation TYStringImage

static NSLayoutManager *layoutManager;
static NSDictionary *tempAttDict;
static NSRange firstCharRange;

+(void)initialize
{
    static BOOL nomore = NO;

    if( nomore )
        return ;
    nomore = YES;
    
    layoutManager = [ [ NSLayoutManager alloc ] init ];
    [ layoutManager addTextContainer:[ [ NSTextContainer alloc ] initWithContainerSize:NSZeroSize ] ];
    [ [ [ NSTextStorage alloc ] initWithString:@"" ] addLayoutManager:layoutManager ]; // dummy
    
    tempAttDict = [ [ NSDictionary dictionaryWithObjectsAndKeys:
                        [ NSColor blackColor ],
                        NSForegroundColorAttributeName,nil ] retain ];
    firstCharRange = NSMakeRange(0,1);
}

+(NSArray*)imagesWithString:(NSString*)aStr
                 attributes:(NSDictionary*)attributes
                     colors:(NSArray*)colorsArray
                 fontHeight:(int)fontHeight
                  fontWidth:(int)fontWidth
                   interval:(int)interval
                     shadow:(BOOL)shadow
{
    NSTextStorage *storage;
    NSMutableDictionary *attDict=[ [ attributes mutableCopy ] autorelease ];
    int i,j,len,cnt;
    //NSRange actRange;
    NSFont *font;
    NSGlyph glyph;
    NSRect bRect;
    NSPoint offset;
    float height;
    NSSize imageSize;
    NSSize fontSize;
    NSMutableArray *array;
    NSAutoreleasePool *pool;

    array = [ NSMutableArray array ];

    if(!aStr || ![ aStr length ]){
        [ array addObject:[ [ [ NSImage alloc ] initWithSize:NSMakeSize(1,1) ] autorelease ] ];
        return array;
    }

    pool = [ [ NSAutoreleasePool alloc ] init ];
        
    font = [ attDict objectForKey:NSFontAttributeName ];
    font = [ NSFont fontWithName:[ font fontName ] size:(fontWidth > fontHeight) ? fontWidth : fontHeight ];
    [ attDict setObject:font forKey:NSFontAttributeName ];
    
    storage = [ [ [ NSTextStorage alloc ] initWithString:aStr attributes:attDict ] autorelease ];
    // サイズ計算
    [ layoutManager replaceTextStorage:storage ];
    len = [ storage length ];
    i = 0;
    glyph = [ layoutManager glyphAtIndex:i ];
    bRect = [ font boundingRectForGlyph:glyph ];
    offset = bRect.origin;

    offset.y = [ font descender ];
    height = [ font ascender ];

    glyph = [ layoutManager glyphAtIndex:len -1 ];
    bRect = [ font boundingRectForGlyph:glyph ];
    imageSize = NSMakeSize(
                           (fontWidth + interval) * (len -1) +bRect.size.width+bRect.origin.x +((offset.x < 0) ? -1 * offset.x : 0),
                           height +((offset.y < 0) ? -1 * offset.y : 0));
    // サイズ0以下（空白文字だけだった場合など）対策
    if(imageSize.width <= 0)
        imageSize.width = 1;
    if(imageSize.height <= 0)
        imageSize.height = 1;
    
    // イメージ作成
    offset.x *= -1;
    offset.y *= -1;
                        
    cnt = [ colorsArray count ];
    
    for(i=0; i < cnt; i++){
        NSImage *newImage;
        NSTextStorage *subStorage;
        NSRange actRange;
        
        newImage = [ [ NSImage alloc ] initWithSize:imageSize ];

        [ storage addAttribute:NSForegroundColorAttributeName
                         value:[ colorsArray objectAtIndex:i ]
                         range:NSMakeRange(0,len) ];
        [ newImage lockFocus ];

        for(j=0; j < len; j++){
            subStorage = [ [ NSTextStorage alloc ]initWithAttributedString:
                [ storage attributedSubstringFromRange:NSMakeRange(j,1) ] ];
            [ layoutManager replaceTextStorage:subStorage ];
            [ layoutManager glyphRangeForCharacterRange:NSMakeRange(0,1)
                                       actualCharacterRange:&actRange ];
            if(shadow){
				[ layoutManager addTemporaryAttributes:tempAttDict
									forCharacterRange:firstCharRange ];
				[ layoutManager drawGlyphsForGlyphRange:actRange
												atPoint:NSMakePoint(offset.x +(fontWidth+interval) *j +SHADOW_TICKNESS,
																offset.y -SHADOW_TICKNESS) ];
				[ layoutManager removeTemporaryAttribute:NSForegroundColorAttributeName
									forCharacterRange:firstCharRange ];
            }
            [ layoutManager drawGlyphsForGlyphRange:actRange
                                            atPoint:NSMakePoint(offset.x +(fontWidth+interval) *j ,
                                                                offset.y ) ];
            [ subStorage release ];
        }
        [ newImage unlockFocus ];

        fontSize=NSMakeSize((fontWidth + interval) * (len -1) +fontWidth,fontHeight);
        newImage = [ [ [ self class ] alloc ] initWithImage:newImage size:imageSize offset:offset ];
        [ array addObject:newImage ];
        [ newImage release ];
    }

    [ pool release ];

    return array;
}

-(id)initWithImage:(NSImage*)aImage size:(NSSize)aSize offset:(NSPoint)point
{
    self = [ super init ];

    image = [ aImage retain ];
    size=aSize;
    origin = point;
    origin.x *= -1;
    origin.y *= -1;

    return self;
}

-(NSPoint)origin { return origin; }
-(void)setOrigin:(NSPoint)point { origin = point; }
-(void)setOriginByValue:(NSValue*)pointValue
{
    [ pointValue getValue:&origin ];
}


-(NSSize)size
{
    return size;
}

- (void)dissolveToPoint:(NSPoint)point fraction:(float)aFloat
{
    point.x += origin.x;
    point.y += origin.y;

    [ image dissolveToPoint:(NSPoint)point fraction:(float)aFloat ];
}

- (void)dissolveToPoint:(NSPoint)point fromRect:(NSRect)rect fraction:(float)aFloat
{
    point.x += origin.x;
    point.y += origin.y;

    [ image dissolveToPoint:(NSPoint)point fromRect:(NSRect)rect fraction:(float)aFloat ];
}

- (void)compositeToPoint:(NSPoint)point operation:(NSCompositingOperation)op
{
    point.x += origin.x;
    point.y += origin.y;

    [ image compositeToPoint:(NSPoint)point operation:(NSCompositingOperation)op ];
}

- (void)compositeToPoint:(NSPoint)point fromRect:(NSRect)rect operation:(NSCompositingOperation)op
{
    point.x += origin.x;
    point.y += origin.y;

    [ image compositeToPoint:(NSPoint)point fromRect:(NSRect)rect operation:(NSCompositingOperation)op ];
}

- (void)compositeToPoint:(NSPoint)point operation:(NSCompositingOperation)op fraction:(float)delta
{
    point.x += origin.x;
    point.y += origin.y;

    [ image compositeToPoint:(NSPoint)point operation:(NSCompositingOperation)op fraction:(float)delta ];
}

- (void)compositeToPoint:(NSPoint)point fromRect:(NSRect)rect operation:(NSCompositingOperation)op fraction:(float)delta
{
    point.x += origin.x;
    point.y += origin.y;

    [ image compositeToPoint:(NSPoint)point operation:(NSCompositingOperation)op fraction:(float)delta ];

}

/* めんどいので保留（えー）
- (void)drawAtPoint:(NSPoint)point fromRect:(NSRect)fromRect operation:(NSCompositingOperation)op fraction:(float)delta;
- (void)drawInRect:(NSRect)rect fromRect:(NSRect)fromRect operation:(NSCompositingOperation)op fraction:(float)delta;
*/
- (BOOL)respondsToSelector:(SEL)aSelector
{
    return ([ super respondsToSelector:aSelector ] ||
            [ image respondsToSelector:aSelector ]);
}

- (void)forwardInvocation:(NSInvocation *)anInvocation
{
    SEL sel = [ anInvocation selector ];
    if([ image respondsToSelector:sel ])
        [ anInvocation invokeWithTarget:image ];
    else
        [ self doesNotRecognizeSelector:sel ];
}

- (NSMethodSignature *)methodSignatureForSelector:(SEL)aSelector
{
    if([ super respondsToSelector:aSelector ])
        return [ super methodSignatureForSelector:aSelector ];
    return [ image methodSignatureForSelector:aSelector ];
}

- (void)dealloc
{
    [image release];
    [super dealloc];
}

@end
