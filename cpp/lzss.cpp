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

#include "lzss.h"
#include <QByteArray>
#include <cstring>

#define EI  8  
#define EJ  4  
#define P   1  /* If match length <= P then output one character */
#define N (1 << EI)  /* buffer size */
#define F ((1 << EJ) + P)  /* lookahead buffer size */

static int bit_mask;
static unsigned char text_buffer[N * 2];
static int stream_size;
static const unsigned char* data_ptr;

static void init_lzss(QFile* fh, size_t encode_size)
{
    stream_size = static_cast<int>(encode_size);
    
    // Read all encoded data into memory
    QByteArray encodedData = fh->read(encode_size);
    data_ptr = reinterpret_cast<const unsigned char*>(encodedData.constData());
    
    bit_mask = 0;
}

static int getbit(int n) /* get n bits */
{
    int i, x;
    static int buf = 0;
    
    x = 0;
    for (i = 0; i < n; i++) {
        if (bit_mask == 0) {
            if (stream_size-- <= 0) return EOF;
            buf = *data_ptr++;
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
QByteArray TYDataWithLzssDecodeFromFile(QFile* fh,
                                        size_t encode_size,
                                        size_t orignal_size)
{
    QByteArray data(static_cast<int>(orignal_size), 0);
    unsigned char* p = reinterpret_cast<unsigned char*>(data.data());
    int i, j, k, r, c;

    init_lzss(fh, encode_size);
    
    for (i = 0; i < N - F; i++) text_buffer[i] = ' ';
    r = N - F;
    while ((c = getbit(1)) != EOF) {
        if (c) {
            if ((c = getbit(8)) == EOF) break;
            *p++ = c;
            text_buffer[r++] = static_cast<unsigned char>(c); r &= (N - 1);
        } else {
            if ((i = getbit(EI)) == EOF) break;
            if ((j = getbit(EJ)) == EOF) break;
            for (k = 0; k <= j + 1; k++) {
                c = text_buffer[(i + k) & (N - 1)];
                *p++ = static_cast<unsigned char>(c);
                text_buffer[r++] = static_cast<unsigned char>(c);
                r &= (N - 1);
            }
        }
    }
    
    return data;
}
