//
//  TYDigitArgment.m
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

#import "TYDigitArgment.h"


@implementation NSNumber(TYDigitArgment)
+(id)argmentWithInt:(int)aInt
{
    return [ NSNumber numberWithInt:aInt ]; 
}

// override
-(id)varID
{
    return [ [ [ [ TYArgment alloc ] init ] autorelease ] varID ];
}

-(TYVarType)varType
{
    return [ [ [ [ TYArgment alloc ] init ] autorelease ] varType ];
}

-(TYArgType)argType
{
    return TYDigitArgType;
}

-(NSNumber*)intNumber
{
    return self;
}

-(NSString*)stringObj
{
    return [ [ [ [ TYArgment alloc ] init ] autorelease ] string ];
}

-(NSString*)rawString
{
    return [ self stringValue ];
}

@end
