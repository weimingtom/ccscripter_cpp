//
//  TYNBZDecompresser.m
//  Tukuyomi
//
//  Created by toveta on Sun Feb 10 2002.
//  Original by NONAKA Kimihiro
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

#import "TYNBZDecompresser.h"

#import <bzlib.h>

@implementation TYNBZDecompresser

static id sharedObject;
+(id)sharedObject
{
    if(!sharedObject)
        sharedObject = [ [ [ self class ] alloc ] init ];

    return sharedObject;
}

/*
 * NBZ
 *
 * +00h : DWORD(BE32) orignal size
 * +04h-: bzip -1 orignal_file
 */
-(NSData*)decodeDataWithFileHandle:(NSFileHandle*)handle length:(int)encode_size
{
    int fd;
    FILE *fp;
    unsigned decode_size;
    unsigned char unused[BZ_MAX_UNUSED];
    BZFILE *bzf;
    unsigned char *top,*p;
    NSMutableData *decodeData;
    unsigned char *unused_tmp;
    int bzerr;
    int nread;
    int nunused;

    decode_size = *(int*)[ [ handle readDataOfLength:4 ] bytes ];
    fd = [ handle fileDescriptor ];
    if((fp = fdopen(fd,"r")) == NULL){
        [ handle closeFile ];
        return nil;
    }
    decodeData = [ NSMutableData dataWithLength:decode_size ];
    p = top = [ decodeData mutableBytes ];

    nunused = 0;
    bzf = BZ2_bzReadOpen(&bzerr, fp, 0, 0, unused, nunused);
    NSAssert((bzf != NULL && bzerr == BZ_OK),@"nbzReadOpen Error");

    while (bzerr == BZ_OK) {
        NSAssert((p < top + decode_size),@"nbzReadSize too large");
        nread = BZ2_bzRead(&bzerr, bzf, p, 5000);
        NSAssert(((bzerr == BZ_OK||bzerr == BZ_STREAM_END) && nread > 0),@"nbzReadError");
        p += nread;
    }
    NSAssert((bzerr == BZ_STREAM_END),@"bzerr is not BZ_STREAM_END");

    BZ2_bzReadGetUnused(&bzerr, bzf, (void **)&unused_tmp, &nunused);
    NSAssert((bzerr == BZ_OK),@"BZ2_bzReadGetUnused");

    BZ2_bzReadClose(&bzerr, bzf);
    NSAssert((bzerr == BZ_OK),@"bzReadClose");

    //NSAssert((nunused == 0),@"nunused not equal 0");

    return decodeData;
}

-(NSBitmapImageRep*)decodeBitmapWithFileHandle:(NSFileHandle*)handle length:(int)encode_size
{
    return [ [ [ NSBitmapImageRep alloc ] initWithData:[ self decodeDataWithFileHandle:handle length:encode_size ] ] autorelease ];
}

@end
