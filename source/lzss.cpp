/*
 *  lzss.h
 *  Tukuyomi
 *
 *  Created by toveta on Mon Jun 17 2002.
 *  Copyright (c) 2002 toveta All rights reserved.
 *
 */
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

#import "lzss.h"

#define EI  8  
#define EJ  4  
#define P   1  /* If match length <= P then output one character */
#define N (1 << EI)  /* buffer size */
#define F ((1 << EJ) + P)  /* lookahead buffer size */

static int bit_mask;
static unsigned char buffer[N * 2];
static int stream_size;
static const unsigned char *fp;

static void init(NSFileHandle *fh,size_t encode_size)
{
    stream_size = encode_size;
    fp = [ [ fh readDataOfLength:encode_size ] bytes ];
    bit_mask = 0;
}

static int getbit(int n) /* get n bits */
{
    int i, x;
    static int buf;
    
    x = 0;
    for (i = 0; i < n; i++) {
        if (bit_mask == 0) {
            if(stream_size-- <= 0) return EOF;
            buf = *fp++;
            bit_mask = 128;
        }
        x <<= 1;
        if (buf & bit_mask) x |= 1;
        bit_mask >>= 1;
    }
    return x;
}

/*
 * NSA アーカイブの変形 LZSS 展開ルーチン (is_compress == 2)
 */
NSData* TYDataWithLzssDecodeFromFile(NSFileHandle *fh,
                                     size_t encode_size,
                                     size_t orignal_size)
{
    NSMutableData *data = [ NSMutableData dataWithLength:orignal_size ]; 
    unsigned char *p = [ data mutableBytes ];
    int i, j, k, r, c;

    init(fh,encode_size);
    
    for (i = 0; i < N - F; i++) buffer[i] = ' ';
    r = N - F;
    while ((c = getbit(1)) != EOF) {
        if (c) {
            if ((c = getbit(8)) == EOF) break;
            *p++ = c;
            buffer[r++] = c;  r &= (N - 1);
        } else {
            if ((i = getbit(EI)) == EOF) break;
            if ((j = getbit(EJ)) == EOF) break;
            for (k = 0; k <= j + 1; k++) {
                c = buffer[(i + k) & (N - 1)];
                *p++ = c;
                buffer[r++] = c;  r &= (N - 1);
            }
        }
    }

    return data;
}
