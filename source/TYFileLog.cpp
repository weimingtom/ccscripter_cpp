//
//  TYFileLog.m
//  Tukuyomi
//
//  Created by toveta on Sat Nov 10 2001.
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

#import "TYFileLog.h"
#import "TY-NSStringAddition.h"
#import "TY_NSDataAddtion.h"
#import <unistd.h>

@implementation TYFileLog
-(id)initWithContentsOfFile:(NSString*)path
{
    NSFileHandle *handle;
    NSMutableData *bufData;
    int fileNum;
    int descripter;
    unsigned char buf;
    int i,ret;

    self = [ super init ];
    
    if(!self)
        return nil;
    
    logSet = [ [ NSMutableSet alloc ] initWithCapacity:1 ];

    // MEMO:ここでログを書き出すディレクトリの書き込み権限チェックを入れる。
    handle = [ NSFileHandle fileHandleForReadingAtPath:path ];
    if(!handle){
        return self;
    }
    descripter = [ handle fileDescriptor ];
    
    /*
    filelogDict = [ [ NSMutableDictionary alloc ] initWithContentsOfFile:
        [ getAppDirectory() stringByAppendingPathComponent:FILELOG_FILENAME ] ];
    */


    bufData = [ NSMutableData dataWithCapacity:1 ];
    while(read(descripter,&buf,1)){
        if(buf == 0x0a)
            break;
        else
            [ bufData appendBytes:&buf length:1 ];
    }
    [ bufData appendBytes:"" length:1 ];
    fileNum = atoi([ bufData bytes ]);

    for(i=0; i < fileNum; i++){
        ret = read(descripter,&buf,1);
        if((!ret)||(buf != '"'))
            break;
            
        bufData = [ NSMutableData dataWithCapacity:1 ];
        
        while(read(descripter,&buf,1)&&(buf!='"')){
            [ bufData appendBytes:&buf length:1 ];
        }
        [ bufData XORMaskToAllBytes:132 ]; // XORマスクをかけられているので戻す。
        [ bufData appendBytes:"" length:1 ];

        [ logSet addObject:[ [ NSString stringWithCSJISString:[ bufData bytes ] ] uppercaseString ] ];
    }
    
    [ handle closeFile ];
    
    return self;
}

-(BOOL)writeToFile:(NSString*)path
{
    char numbuf[12];
    NSMutableData *writeData;
    NSMutableData *data;
    int count;
    NSEnumerator *enm;
    id temp;
    
    writeData = [ NSMutableData dataWithCapacity:2 ];
    count = [ logSet count ];
    sprintf(numbuf,"%d\n",count);
    [ writeData appendBytes:numbuf length:strlen(numbuf) ];
    
    enm = [ logSet objectEnumerator ];
    while((temp = [ enm nextObject ]) != nil){
        [ writeData appendBytes:"\"" length:1 ];
        data = [ [ [ temp sjisData ] mutableCopy ] autorelease ];
        [ data XORMaskToAllBytes:132 ];
        [ writeData appendData:data ];
        [ writeData appendBytes:"\"" length:1 ];
    }
 
    return [ writeData writeToFile:path atomically:YES ];
}

-(void)addTYFileLog:(TYFileLog*)margeLog
{
    [ logSet unionSet:margeLog->logSet ];
}

-(void)addLog:(NSString*)filename
{
    [ logSet addObject:[ filename uppercaseString ] ];
}

-(BOOL)isRead:(NSString*)filename
{
    return [ logSet containsObject:[ filename uppercaseString ] ];
}

-(void)dealloc
{
    [ logSet release ];
    [super dealloc];
}
@end
