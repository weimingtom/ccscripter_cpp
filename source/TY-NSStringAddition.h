//
//  TY-NSStringAddition.h
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

#import <Foundation/Foundation.h>


// Mac OS X日本語環境のデフォルトエンコーディングを使用していると機種依存文字で死ぬので。
// 主にそのへんをなんとかする拡張
@interface NSString (TY_NSStringAddition)
+(NSString*)stringWithCSJISString:(const char*)cString;
+(NSString*)stringWithCSJISString:(const char*)cString length:(unsigned int)aInt;
-(const char*)cSJISString;
-(NSMutableData*)getCSJISData;
-(NSData*)sjisData;
-(unsigned)cSJISStringLength;
-(void)getCSJISString:(char *)buffer;
-(NSString*)substringSJISRange:(NSRange)range;
@end

extern NSString* getBackSlashString();