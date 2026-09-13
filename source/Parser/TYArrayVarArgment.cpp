//
//  TYArrayVarArgment.m
//  Tukuyomi
//
//  Created by toveta on Sun Apr 13 2003.
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

#import "TYArrayVarArgment.h"


@implementation TYArrayVarArgment
+(id)argmentWithVarID:(id)aObj arrayIndex:(NSArray*)aArray
{
    return [ [ [ self alloc ] initWithVarID:aObj arrayIndex:aArray ] autorelease ];
}

-(id)initWithVarID:(id)aObj arrayIndex:(NSArray*)aArray
{
    NSEnumerator *enu;
    id temp;
    
    self = [ super init ];
    
    varID = [ [ NSMutableArray arrayWithCapacity:[ aArray count ]+1 ] retain ];
    
    [ varID addObject:[ aObj intNumber ] ];
    enu = [ aArray objectEnumerator ];
    while(temp = [ enu nextObject ]){
        [ varID addObject:[ temp intNumber ] ];
    }
    
    return self;
}

// override
-(TYVarType)varType
{
    return TYArrayVarType;
}

//-(TYArgType)argType

@end
