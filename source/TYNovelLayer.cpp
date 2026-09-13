//
//  TYNovelLayer.m
//  Tukuyomi
//
//  Created by toveta on Mon Dec 31 2001.
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

#import "TYNovelLayer.h"
#import "TYStageManager.h"
#import "TYLookbackLayer.h"

NSString* const TYPathVoiceLookbackData=@"TYPathVoiceLookbackData";
NSString* const TYPointVoiceLookbackData=@"TYPointVoiceLookbackData";

@implementation TYNovelLayer

+(void)initialize
{
    [ self setVersion:1 ];
}

- (id)initNovelLayer
{
    self = [ super init ];
    
    textArray = [ [ NSMutableArray array ] retain ];
    bufferImage = [ [ NSImage alloc ] initWithSize:TYVirtualScreenSize() ];
    tempString = [ [ NSMutableAttributedString alloc ] initWithString:@"" ];
    voiceArray = [ [ NSMutableArray array ] retain ];
    needRefresh = YES;

    [ [ NSNotificationCenter defaultCenter ] addObserver:self selector:@selector(playVoice:)  name:TYVoicePlayNotification object:nil ];
    
    return self;
}

-(void)recache
{
    if(!needRefresh)
        return;
    needRefresh = NO;
    
    [ bufferImage lockFocus ];
    //NSRectFillUsingOperation(TYVirtualScreenRect(),NSCompositeClear);
    [ self drawImage ];
    [ bufferImage unlockFocus ];
}

-(void)setManager:(id)manager
{
    delegate = manager;
}

-(void)pushAttributedString:(NSAttributedString*)str
{
    needRefresh = YES;

    [ tempString appendAttributedString:str ];
}

-(void)pushNewLine
{
    needRefresh = YES;
    
    [ textArray addObject:tempString ];
    [ tempString release ];
    tempString = [ [ NSMutableAttributedString alloc ] initWithString:@"" ];
    nowColumn=0;
}

-(TYLookbackLayer*)lookbackObject
{        
    return [ [ [ TYLookbackLayer alloc ] initWithStrings:[ self stringArray ]
                                                  voices:[ self voiceArray ] ] autorelease ];
}

-(NSArray*)stringArray
{
    NSMutableArray *array;
    NSEnumerator *enu;
    NSAttributedString *temp;
    
    array = [ NSMutableArray array ];
    enu = [ textArray objectEnumerator ];
    while(temp = [ enu nextObject ]){
        [ array addObject:[ temp string ] ];
    }
    if([ tempString length ])
        [ array addObject:[ tempString string ] ];

    return array;
}

-(NSArray*)voiceArray
{
    return ([ voiceArray count ]) ? voiceArray : nil;
}

-(void)drawImage
{
    id temp;
    NSRange range;
    NSEnumerator *enm;
    int length,column,row;

    if(!isDrawTextWindow){
        [ delegate drawTextWindow ];
        isDrawTextWindow = YES;
    }
    
    enm = [ textArray objectEnumerator ];
    range.length = 1;
    column=0;
    row=0;
    while((temp = [ enm nextObject ]) != nil){
        if(nowRow <= row){
            length = [ (NSAttributedString*)temp length ];
            for(range.location=0; range.location < length; range.location++,column++){
                [ delegate drawAttStr:[ temp attributedSubstringFromRange:range ] atColumn:column atRow:row ];
            }
        }
        column=0;
        row++;
    }

    length = [ tempString length ];
    column = nowColumn;
    for(range.location=nowColumn; range.location < length; range.location++,column++){
        [ delegate drawAttStr:[ tempString attributedSubstringFromRange:range ] atColumn:column atRow:row ];
    }
    
    nowRow = row;
    nowColumn = column;
}

- (void)forwardInvocation:(NSInvocation *)anInvocation
{
    SEL sel = [ anInvocation selector ];
    if([ bufferImage respondsToSelector:sel ])
        [ anInvocation invokeWithTarget:bufferImage ];
    else
        [ self doesNotRecognizeSelector:sel ];
}

- (NSMethodSignature *)methodSignatureForSelector:(SEL)aSelector
{
    if([ super respondsToSelector:aSelector ])
        return [ super methodSignatureForSelector:aSelector ];
    return [ bufferImage methodSignatureForSelector:aSelector ];
}

-(void)playVoice:(NSNotification*)aNotification
{
    [ voiceArray addObject:[ NSDictionary dictionaryWithObjectsAndKeys:
        [ aNotification object ],
        TYPathVoiceLookbackData,
        [ NSValue valueWithPoint:NSMakePoint([ tempString length ],[ textArray count ]) ],
        TYPointVoiceLookbackData,nil ] ];
}

-(NSString*)getText
{    
    return [ [ self stringArray ] componentsJoinedByString:@"" ];
}

- (void)dealloc
{
    [ bufferImage release ];
    [ textArray release ];
    [ tempString release ];
    [ voiceArray release ];
    [ [ NSNotificationCenter defaultCenter ] removeObserver:self ];
    [super dealloc];
}

// implementation of NSCoding
- (void)encodeWithCoder:(NSCoder *)aCoder
{
    [ aCoder encodeObject:textArray ];
    [ aCoder encodeObject:tempString ];
    [ aCoder encodeObject:voiceArray ];
}

- (id)initWithCoder:(NSCoder *)aDecoder
{    
    textArray = [ [ aDecoder decodeObject ] mutableCopy ];
    tempString = [ [ aDecoder decodeObject ] mutableCopy ];
    if([ aDecoder versionForClassName:@"TYNovelLayer" ] >= 1)
        voiceArray = [ [ aDecoder decodeObject ] mutableCopy ];
    
    bufferImage = [ [ NSImage alloc ] initWithSize:TYVirtualScreenSize() ];
    needRefresh = YES;
    
    return self;
}

@end
