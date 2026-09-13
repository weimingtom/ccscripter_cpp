//
//  TYScripterValues.h
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

#import <Foundation/Foundation.h>
#import "TYSaveDataCoding.h"

#define NUM_OF_VALUES 10000

// ユーザー定義変数を格納するオブジェクトです。
@interface TYScripterValues : NSObject < TYSaveDataCoding >{
    int values[NUM_OF_VALUES];
    NSString *strValues[NUM_OF_VALUES];
    NSMutableDictionary *arrayValues;
    NSMutableDictionary *limitDict;
}


+(id)ScripterValues;

+(void)setMinGlobalNo:(int)aInt;

-(id)initScripterValues;
-(id)getStringValue:(id)idno;
-(id)getIntValue:(id)idno;
-(void)setStringValue:(id)idno object:(NSString*)setString;
-(void)setIntValue:(id)idno object:(NSNumber*)setNumber;
-(void)setArrayLine:(id)idno values:(NSArray*)aArray;
-(void)setIntLimitID:(id)idno low:(id)aLow high:(id)aHigh;

-(void)defineArrayValue:(NSArray*)aArray;

-(BOOL)loadGlobalValues:(NSString*)path;
-(BOOL)saveGlobalValues:(NSString*)path;
-(void)margeGlobalValues:(TYScripterValues*)margeValues;
-(void)resetGame;
@end