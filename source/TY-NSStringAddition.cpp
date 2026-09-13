//
//  TY-NSStringAddition.m
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

#import "TY-NSStringAddition.h"


@implementation NSString (TY_NSStringAddition)
+(NSString*)stringWithCSJISString:(const char*)cString
{
    NSString *str;
    
    str = [ [ [ NSString alloc ] initWithData:[ NSData dataWithBytes:cString length:strlen(cString) ] 
                encoding:NSShiftJISStringEncoding ] autorelease ];
    if(str){
        return str;
    } else {
        return @"";
    }
}

+(NSString*)stringWithCSJISString:(const char*)cString length:(unsigned int)aInt
{
    NSString *str = [ [ [ NSString alloc ] initWithData:[ NSData dataWithBytes:cString length:aInt ] 
                encoding:NSShiftJISStringEncoding ] autorelease ];
    if(str){
        return str;
    } else {
        return @"";
    }
    
}

-(const char*)cSJISString
{
    char nullbyte = '\0';
    NSData *data;
    NSMutableData *mdata;
    data = [ self dataUsingEncoding:NSShiftJISStringEncoding ];
    mdata = [ NSMutableData dataWithData:data ];
    [ mdata appendBytes:&nullbyte length:1 ];
    return [ mdata bytes ];
}

-(NSMutableData*)getCSJISData
{
    char nullbyte = '\0';
    NSData *data;
    NSMutableData *mdata;
    data = [ self dataUsingEncoding:NSShiftJISStringEncoding ];
    mdata = [ NSMutableData dataWithData:data ];
    [ mdata appendBytes:&nullbyte length:1 ];
    
    return mdata;
}

-(NSData*)sjisData
{
    return [ self dataUsingEncoding:NSShiftJISStringEncoding ];
}

-(unsigned)cSJISStringLength
{
    return [ [ self dataUsingEncoding:NSShiftJISStringEncoding ] length ];
}

-(void)getCSJISString:(char *)buffer
{
    NSData *data;
    data = [ self dataUsingEncoding:NSShiftJISStringEncoding ];
    [ data getBytes:buffer ];
    *(buffer +[ data length ]) = '\0';
}

-(NSString*)substringSJISRange:(NSRange)range
{
    NSData *scrData;
    NSMutableData *dstData;
    const char *ptr;
    int strLength;
    
    scrData = [ self sjisData ];
    ptr = [ scrData bytes ];
    
    ptr += range.location;
    strLength = [ scrData length ] -range.location;
    // 自身の長さを超える値を指定されたら切りつめる
    if(range.length > strLength)
        range.length = strLength;
    
    dstData = [ NSMutableData dataWithBytes:ptr length:range.length ];
    return [ [ [ NSString alloc ] initWithData:dstData encoding:NSShiftJISStringEncoding ] autorelease ];
}

@end

NSString* getBackSlashString()
{
    char b = '\\';
    
    return [ [ [ NSString alloc ] initWithData:[ NSData dataWithBytes:&b length:1 ] encoding:NSShiftJISStringEncoding ] autorelease ];
}
