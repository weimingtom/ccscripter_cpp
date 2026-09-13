//
//  TYBitOperator.m
//  Tukuyomi
//
//  Created by toveta on Mon Jun 17 2002.
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

#import "TYBitOperator.h"
#import <unistd.H>

@implementation TYBitOperator
-(id)initWithFileHandle:(NSFileHandle*)f size:(unsigned int)s
{
    self = [ super init ];

    fh = [ f retain ];
    size = s;

    mask = 0x100;
    tmp = *(unsigned char*)[ [ fh readDataOfLength:1 ] bytes ];

    return self;
}

-(int)get
{
    mask >>= 1;
    if (mask == 0x00) {
        NSAssert((size != 0),@"Over Limit Size");
        size--;
        tmp = *(unsigned char*)[ [ fh readDataOfLength:1 ] bytes ];
        mask = 0x80;
    }
    return (tmp & mask) ? 1 : 0;    
}

-(unsigned int)getWithLength:(int)n
{
    unsigned int v;

    NSAssert((n > 0 && n <= 32),@"invalid bit length");

    for (v = 0; n > 0; n--) {
        v <<= 1;
        if ([ self get ])
            v |= 1;
    }
    return v;
    
}

- (void)dealloc
{
    [ fh release ];
    [super dealloc];
}

TYCoreBitOperater TYCreateCoreBitOperater(NSFileHandle* f,unsigned int s)
{
    TYCoreBitOperater mySelf;
    int result;
    
    mySelf = malloc(sizeof(struct tyCoreBitOperater));
    if(mySelf==NULL)
        return NULL;
        
    mySelf->fd = [ f fileDescriptor ];
    //mySelf->size = s;
    mySelf->mask = 0x100;
    result = read(mySelf->fd,&mySelf->tmp,1);
    if(!result){
        TYReleaseCoreBitOperater(mySelf);
        return NULL;
    }
    
    return mySelf;
}

__inline__ int TYGetCoreBitOperater(TYCoreBitOperater mySelf)
{
    mySelf->mask >>= 1;
    if (mySelf->mask == 0x00) {
        //NSCAssert((mySelf->size != 0),@"Over Limit Size");
        //mySelf->size--;
        read(mySelf->fd,&mySelf->tmp,1);
        mySelf->mask = 0x80;
    }
    return (mySelf->tmp & mySelf->mask) ? 1 : 0;    
}

__inline__ unsigned int TYGetBitsCoreBitOperater(TYCoreBitOperater mySelf,int n)
{
    unsigned int v=0;

    //NSCAssert((n > 0 && n <= 32),@"invalid bit length");

    for (v = 0; n > 0; n--) {
        v <<= 1;
        
        mySelf->mask >>= 1;
        if (mySelf->mask == 0x00) {
            read(mySelf->fd,&mySelf->tmp,1);
            mySelf->mask = 0x80;
        }
        if (mySelf->tmp & mySelf->mask)
        //if (TYGetCoreBitOperater(mySelf))
            v |= 1;
    }
    return v;    
}

void TYReleaseCoreBitOperater(TYCoreBitOperater mySelf)
{
    free(mySelf);
}

@end
