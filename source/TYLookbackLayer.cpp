//
//  TYLookbackLayer.m
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

#import "TYLookbackLayer.h"
#import "TYStageManager.h"

@implementation TYLookbackLayer

static NSImage *lookBackBuffer; // 共用のバッファ、全ての回想ページでこれを使い回す

-(id)initWithStrings:(NSArray*)aArray
              voices:(NSArray*)vArray
{
    self = [ super init ];
    
    textArray = [ aArray retain ];

    voiceArray = [ vArray retain ];

    tempString = [ [ NSMutableAttributedString alloc ] initWithString:@"" ];

    needRefresh = YES;

    return self;
}

-(void)recache
{
    nowColumn = 0;
    nowRow = 0;
    
    if(lookBackBuffer)
        [ lookBackBuffer release ];
    lookBackBuffer = [ [ NSImage alloc ] initWithSize:TYVirtualScreenSize() ];
    
    [ lookBackBuffer lockFocus ];
    [ self drawImage ];
    [ lookBackBuffer unlockFocus ];
}

-(void)drawImage
{
    IMP imp;
    SEL sel;
    id temp;
    NSRange range;
    NSEnumerator *enm;
    int length,column,row;
    
    {
        sel = @selector( drawAttStr: atColumn: atRow: );
        imp = [ delegate methodForSelector:sel ]; 
    }
    
    enm = [ textArray objectEnumerator ];
    range.length = 1;
    column=0;
    row=0;
    while((temp = [ enm nextObject ]) != nil){
        if(nowRow <= row){
            length = [ (NSAttributedString*)temp length ];
            for(range.location=0; range.location < length; range.location++,column++){
                (void(*)( id , SEL , ...))imp(delegate,sel,[ temp attributedSubstringFromRange:range ],column,row);
                //[ delegate drawAttStr:[ temp attributedSubstringFromRange:range ] atColumn:column atRow:row ];
            }
        }
        column=0;
        row++;
    }
}


-(void)setAttribute:(NSDictionary*)aDict
{
    NSMutableArray *newArray;
    NSEnumerator *enu;
    NSString *temp;

    if(attributed)
        return;
    
    newArray = [ NSMutableArray array ];
    enu = [ textArray objectEnumerator ];
    while(temp = [ enu nextObject ]){
        [ newArray addObject:[ [ [ NSAttributedString alloc ] initWithString:temp attributes:aDict ] autorelease ] ];
    }

    [ textArray release ];
    textArray = [ newArray retain ];
    
    attributed = YES;
}

- (void)forwardInvocation:(NSInvocation *)anInvocation
{
    SEL sel = [ anInvocation selector ];
    if([ lookBackBuffer respondsToSelector:sel ])
        [ anInvocation invokeWithTarget:lookBackBuffer ];
    else
        [ self doesNotRecognizeSelector:sel ];
}

- (NSMethodSignature *)methodSignatureForSelector:(SEL)aSelector
{
    if([ super respondsToSelector:aSelector ])
        return [ super methodSignatureForSelector:aSelector ];
    return [ lookBackBuffer methodSignatureForSelector:aSelector ];
}

-(NSArray*)stringArray
{
    if(attributed)
        return [ super stringArray ];
    else
        return textArray;
}

// implementation of NSCoding
- (void)encodeWithCoder:(NSCoder *)aCoder
{
    [ aCoder encodeObject:[ self stringArray ] ];
    [ aCoder encodeObject:voiceArray ];
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
    textArray = [ [ aDecoder decodeObject ] mutableCopy ];
    voiceArray = [ [ aDecoder decodeObject ] mutableCopy ];

    needRefresh = YES;
    
    return self;
}


@end
