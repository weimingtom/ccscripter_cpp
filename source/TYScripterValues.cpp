//
//  TYScripterValues.m
//  Tukuyomi
//
//  Created by toveta on Mon Jun 11 2001.
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

#import "TYScripterValues.h"
#import "TY-NSStringAddition.h"
#import <unistd.h>
#import "TYArrayValue.h"
#import "TYMiscUtil.h"

static int MIN_GLOBAL_IDNO = 200; // これ以上のidnoならグローバル変数
#define MAX_GLOBAL_IDNO 4095 // ここまでのidnoならグローバル変数

@implementation TYScripterValues
+(id)ScripterValues
{
    return [ [ [ TYScripterValues alloc ] initScripterValues ] autorelease ];
}

+(void)setMinGlobalNo:(int)aInt
{
    MIN_GLOBAL_IDNO = aInt;
}

-(id)initScripterValues
{
    int i;
    NSString *blankStr=@"";
    
    self = [ super init ];
    
    for(i=0; i < NUM_OF_VALUES; i++){
        strValues[i] = blankStr;
        [ strValues[i] retain ];
    }
    
    arrayValues = [ [ NSMutableDictionary alloc ] initWithCapacity:1 ];

    return self;
}

-(id)encodeWithSaveData
{
    // ローカル変数の値をNSDataに詰めて返す。形式はGlobalと同じ。
    NSMutableData *saveData;
    int i;
    unsigned char buf[4];
    NSMutableData *bufData;

    saveData = [ NSMutableData dataWithCapacity:MIN_GLOBAL_IDNO *5 ];

    for(i=0; i < MIN_GLOBAL_IDNO; i++){
        // 数字変数の格納。リトルエンディアンのint型で格納する。
        buf[0] = values[i] & 0xFF;
        buf[1] = ( values[i] >> 8 ) & 0xFF;
        buf[2] = ( values[i] >> 16 ) & 0xFF;
        buf[3] = ( values[i] >> 24 ) & 0xFF;

        [ saveData appendBytes:buf length:4 ];

        // 文字列変数の格納。C文字列に変換して格納する。
        bufData = [ strValues[i] getCSJISData ];
        [ saveData appendBytes:[ bufData bytes ] length:[ bufData length] ];
    }

    return [ NSArray arrayWithObjects:saveData,arrayValues,nil ];
}

-(void)decodeWithSaveData:(id)aObject
{
    // encodeされたデータからローカル変数を復元する。
    int i,len;
    const unsigned char *data;

    if([ aObject respondsToSelector:@selector(objectAtIndex:) ]){
        data = [ [ aObject objectAtIndex:0 ] bytes ];
        [ arrayValues release ];
        arrayValues = [ [ aObject objectAtIndex:1 ] retain ];
    } else {
        data = [ aObject bytes ];
    }
    
    for(i=0; i < MIN_GLOBAL_IDNO ; i++){
        // 数字変数の取得。リトルエンディアンのint型で格納されている。
        values[i] = data[0] + (data[1] << 8)  + (data[2] << 16)  + (data[3] << 24);
        data+=4;
        //        debug = values[i];

        // 文字変数の取得。C文字列で格納されている。

        len = strlen(data);
        [ strValues[i] release ];
        strValues[i] = [ NSString stringWithCSJISString:data ];
        [ strValues[i] retain ];
        data+=len+1;
    }
}

-(NSString*)getStringValue:(id)idno
{
    return strValues[[ idno intValue ]];
}

-(NSNumber*)getIntValue:(id)idno
{
    if([ idno respondsToSelector:@selector(intValue) ]){
        int intId=[ idno intValue ];
        
        NSAssert1(((intId >= 0)&&(intId < NUM_OF_VALUES)),@"int value referance over flow %d",intId);
        return [ NSNumber numberWithInt:values[intId] ];
    } else {
        // 配列変数の場合
        TYArrayValue *array;
        
        array = [ arrayValues objectForKey:[ idno objectAtIndex:0 ] ];
        return [ array getNumOfAddress:[ idno subarrayFromIndex:1 ] ];
    }
}

-(void)setStringValue:(id)idno object:(NSString*)setString
{    
    int intId = [ idno intValue ];

    NSAssert(((intId >= 0)&&(intId < NUM_OF_VALUES)),@"string value referance over flow");  
      
    [ strValues[intId] autorelease ];
    strValues[intId] = setString;
    [ strValues[intId] retain ];
}

-(void)setIntValue:(id)idno object:(NSNumber*)setNumber
{
    if([ idno respondsToSelector:@selector(intValue) ]){
        NSArray *limitArray;
        int intId = [ idno intValue ];

        NSAssert1(((intId >= 0)&&(intId < NUM_OF_VALUES)),@"int value referance over flow %d",intId);
        values[intId] = [ setNumber intValue ];
        if(limitDict && (limitArray = [ limitDict objectForKey:[ idno stringValue ] ])) {
            if(values[intId] < [ [ limitArray objectAtIndex:0 ] intValue ])
                values[intId] = [ [ limitArray objectAtIndex:0 ] intValue ];
            else if(values[intId] > [ [ limitArray objectAtIndex:1 ] intValue ])
                values[intId] = [ [ limitArray objectAtIndex:1 ] intValue ];                
        }
    } else {
        TYArrayValue *value;
        value = [ arrayValues objectForKey:[ idno objectAtIndex:0 ] ];
        [ value setNum:setNumber address:[ idno subarrayFromIndex:1 ] ];
    }
}

-(void)setArrayLine:(id)idno values:(NSArray*)aArray
{
    TYArrayValue *value;
    
    value = [ arrayValues objectForKey:[ idno objectAtIndex:0 ] ];
    [ value setNumbers:aArray  address:[ idno subarrayFromIndex:1 ] ];
}

-(void)defineArrayValue:(NSArray*)aArray
{
    NSNumber *idNo;
    NSMutableArray *incArray;
    
    idNo = [ aArray objectAtIndex:0 ];
    
    // 添字+1の長さを実際には確保する必要があるため、ここで加工する
    {
        NSEnumerator *enu = [ aArray objectEnumerator ];
        id tmp;
        
        incArray = [ NSMutableArray arrayWithCapacity:[ aArray count ] -1 ];
        
        [ enu nextObject ];
        while(tmp = [ enu nextObject ]){
            [ incArray addObject:[ NSNumber numberWithInt:[ tmp intValue ] +1 ] ];
        }

    }
    
    [ arrayValues setObject:[ [ TYArrayValue alloc ] initWithID:[ idNo intValue ] 
                                                         limits:incArray ] 
                     forKey:idNo ];
}

-(void)setIntLimitID:(id)idno low:(id)aLow high:(id)aHigh
{
    if(!limitDict) {
        limitDict = [ [ NSMutableDictionary dictionary ] retain ];
    }
    
    [ limitDict setObject:[ NSArray arrayWithObjects:aLow,aHigh,nil ] forKey:[ idno stringValue ] ];
}

-(BOOL)loadGlobalValues:(NSString*)path
{
    NSFileHandle *handle;
    int descripter;
    int i,ret;
    unsigned char buf[4];
    NSMutableData *bufData;
    
    handle = [ NSFileHandle fileHandleForReadingAtPath:path ];
    if(!handle)
        return NO;
        
    descripter = [ handle fileDescriptor ];
    for(i=MIN_GLOBAL_IDNO; i < MAX_GLOBAL_IDNO ; i++){
        
        bufData = [ [ NSMutableData alloc ] initWithCapacity:1 ];
        
        // 数字変数の取得。リトルエンディアンのint型で格納されている。
        read(descripter,buf,4);
        values[i] = buf[0] + (buf[1] << 8)  + (buf[2] << 16)  + (buf[3] << 24);
        
        // 文字変数の取得。C文字列で格納されている。
        while(1){
            ret = read(descripter,buf,1);
            [ bufData appendBytes:buf length:1 ];
            if(!buf[0] || !ret){
                break;
            }
        }
        if([ bufData length ]){
            [ strValues[i] release ];
            strValues[i] = [ NSString stringWithCSJISString:[ bufData bytes ] ];
            [ strValues[i] retain ];
        }
        [ bufData release ];
    }
    
    //close(descripter);
    [ handle closeFile ];
    
    return YES;
}

-(BOOL)saveGlobalValues:(NSString*)path
{
    NSMutableData *saveData;
    int i;
    unsigned char buf[4];
    NSMutableData *bufData;
        
    saveData = [ NSMutableData dataWithCapacity:(MAX_GLOBAL_IDNO -MIN_GLOBAL_IDNO) *5 ];
    
    for(i=MIN_GLOBAL_IDNO; i < MAX_GLOBAL_IDNO; i++){
        // 数字変数の格納。リトルエンディアンのint型で格納する。
        buf[0] = values[i] & 0xFF;
        buf[1] = ( values[i] >> 8 ) & 0xFF;
        buf[2] = ( values[i] >> 16 ) & 0xFF;
        buf[3] = ( values[i] >> 24 ) & 0xFF;
        
        [ saveData appendBytes:buf length:4 ];
        
        // 文字列変数の格納。C文字列に変換して格納する。
        bufData = [ strValues[i] getCSJISData ];
        [ saveData appendBytes:[ bufData bytes ] length:[ bufData length] ];
    }
    
    [ saveData writeToFile:path atomically:YES ];
    
    return YES;
}

-(void)margeGlobalValues:(TYScripterValues*)margeValues
{
    int i;
    
    for(i=MIN_GLOBAL_IDNO; i < MAX_GLOBAL_IDNO; i++){
        if(!values[i]){
            values[i] = margeValues->values[i];
        }
        if(![ strValues[i] length ]){
            [ strValues[i] release ];
            strValues[i] = margeValues->strValues[i];
            [ strValues[i] retain ];
        }
    }
}

-(void)resetGame
{
    int i;
    NSString *zero=@"";

    for(i=0; i < MIN_GLOBAL_IDNO ; i++){
        values[i] = 0;
        [ strValues[i] release ];
        strValues[i] = zero;
        // const string はreference countに関係なくメモリに常駐するのでretainする必要が無い。
    }
}

-(void)dealloc
{
    int i;
    
    for(i=0; i < NUM_OF_VALUES; i++){
        [ strValues[i] release ]; 
    }
    [ arrayValues release ];
    [ limitDict release ];

    [ super dealloc ];
}

@end
