//
//  TY_NSDataAddtion.m
//  Tukuyomi
//
//  Created by toveta on Wed Oct 24 2001.
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

#import "TY_NSDataAddtion.h"

@implementation NSData(TYBitAccess)

-(int)getBitAtIndex:(unsigned int)aIndex
{
    if([ self length ] < (aIndex >> 3))
        return 0;

    return *((const unsigned char*)[ self bytes ]+(aIndex >> 3)) & (1 << (aIndex & 0x7));
}

@end

@implementation NSMutableData(TYBitAccess)
-(void)setBit:(int)aBit atIndex:(unsigned int)aIndex
{
    int needLength = aIndex >> 3;
    unsigned char* setptr;
    
    if([ self length ] < needLength){
        [ self setLength:needLength ];
        if(!aBit)
            return ;
        *((unsigned char*)[ self mutableBytes ]+(aIndex >> 3)) = 1 << (aIndex & 0x7);
    } else {
        setptr = (unsigned char*)[ self mutableBytes ]+(aIndex >> 3);
        if(aBit)
            *setptr |= 1 << (aIndex & 0x7);
        else
            *setptr &= ~(1 << (aIndex & 0x7));
    }
}

@end


@implementation NSMutableData(TY_NSDataAddtion)

-(void)XORMaskToAllBytes:(unsigned char)code
{
    int i,len;
    unsigned char* byte;
    
    byte = [ self mutableBytes ];
    
    for(i = 0,len = [ self length ]; i < len; i++,byte++){
        *byte = *byte ^ code;
    }
}

@end
