//
//  TYArgmentArray.h
//  Tukuyomi
//
//  Created by toveta on Wed Oct 30 2002.
//  Copyright (c) 2002 toveta All rights reserved.
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

#import <Foundation/Foundation.h>

typedef enum {
    TYParseModeNormal=0,
    TYParseModeCondition,
    TYParseModeLoop
} TYParseMode;

// スクリプト行を解析し、指定されたインデックスのトークンを返すクラス
@interface TYArgmentArray : NSObject {
    NSMutableArray *parsedArgments; // 転送
    BOOL isParsed;
    TYParseMode parseMode;
    NSData *sourceData;
}
+(id)arrayWithCString:(char*)aCStr command:(char*)cmdCStr;
-(id)initWithCString:(char*)aCStr command:(char*)cmdCStr;

// パースモードは外から指定する
-(void)setParseMode:(TYParseMode)mode;

-(void)parse; // private

@end

// MEMO:ScriptEngine側の実装によってはNSArrayをカテゴリで拡張する必要があるかも?
