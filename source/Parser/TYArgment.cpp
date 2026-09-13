//
//  TYArgment.m
//  YLLinkTest
//
//  Created by toveta on Sun Apr 06 2003.
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

#import "TYArgment.h"


@implementation TYArgment

-(id)varID
{
    [ NSException raise:@"TYNSCRInvalidContextException" format:@"not var" ];
    
    return nil;
}

-(TYVarType)varType
{
    [ NSException raise:@"TYNSCRInvalidContextException" format:@"not var" ];
    
    return nil;
}

-(TYArgType)argType
{
    [ NSException raise:@"TYNotImpelementException" format:@"not implement argType:" ];
    
    return nil;
}

-(NSNumber*)intNumber
{
    [ NSException raise:@"TYNSCRInvalidContextException" format:@"not int" ];
    
    return nil;
}

-(int)intValue
{
    return [ [ self intNumber ] intValue ];
}

-(NSString*)stringObj
{
    [ NSException raise:@"TYNSCRInvalidContextException" format:@"not string" ];
    
    return nil;
}

-(NSString*)rawString
{
    [ NSException raise:@"TYNSCRInvalidContextException" format:@"not rawString" ];
    
    return nil;
}

@end

BOOL TYCompareArgment(id left,id right ,TYOpType op)
{
    NSComparisonResult result;
    BOOL flg=0;

    // MEMO:引数の型チェックが必要
    // 左辺の文字列エイリアス、型の異る比較がエラーとなる。
    switch ([ left argType ]) {
    case TYConstStringArgType:
    case TYStringVarArgType:
    case TYFchkArgType:
        result = [ [ left stringObj ] compare:[ right stringObj ] ];
        break;
    case TYDigitArgType:
    case TYIntVarArgType:
    case TYAliasArgType:
    default:
        result = [ [ left intNumber ] compare:[ right intNumber ] ];
        break;
    }
    
    if(result == NSOrderedSame)
        flg = 2;
    else if(result == NSOrderedAscending)
        flg = 1;
    else if(result == NSOrderedDescending)
        flg = 4;
    
    return (flg & op) ? YES : NO;
}
