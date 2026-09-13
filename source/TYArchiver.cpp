//
//  TYArchiver.m
//  Tukuyomi
//
//  Created by toveta on Wed Aug 22 2001.
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

#import "TYArchiver.h"
#import "TYSarArchiver.h"
#import "TYNsaArchiver.h"

@implementation TYArchiver
+(id)archiverWithContentsOfFile:(NSString*)path
{
    return [ [ [ [ self class ] alloc ] initWithContentsOfFile:path ] autorelease ];
}

-(id)initWithContentsOfFile:(NSString*)path
{
    id temp;
    NSString *excstr;
    
    // selfはそのまま破棄される
    self = [ [ super init ] autorelease ];
    
    /* ファイルの拡張子を取得 */
    excstr = [ path pathExtension ];
    
    if([ excstr compare:@"sar" options:NSCaseInsensitiveSearch ] == NSOrderedSame ){
        // sar形式
        temp = [ [ TYSarArchiver alloc ] initWithContentsOfFile:path ];
        return temp;
    } else if([ excstr compare:@"nsa" options:NSCaseInsensitiveSearch ] == NSOrderedSame ){
        temp = [ [ TYNsaArchiver alloc ] initWithContentsOfFile:path ];
        return temp;
    }
    
    NSLog(@"Tukuyomi is not supporting this type of archive.[%@]",path);
    return nil;
}

-(NSFileHandle*)fileHandleWithPath:(NSString*)aPath length:(int*)length;
{
    return nil;
}

-(NSData*)unArchivedFile:(NSString*)address
{
    return nil;
}

@end
