//
//  spb.m
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

#include "spb.h"
#include <QImage>
#include <QByteArray>
#include <cstring>
#include <cassert>

#define BPP 3

// Helper macros for bit extraction
#define BIT_GETN(d, n, bits_ptr, src_ptr, remain_bit, encode_size) \
    if (remain_bit < n) { \
        if (src_ptr >= data + encode_size) { \
            break; \
        } \
        *bits_ptr = (*bits_ptr << 8) | *src_ptr++; \
        remain_bit += 8; \
    } \
    remain_bit -= n; \
    d = (*bits_ptr >> remain_bit) & ((1 << n) - 1);

#define BIT_GET8(d, bits_ptr, src_ptr, remain_bit, encode_size) \
    if (remain_bit < 8) { \
        if (src_ptr >= data + encode_size) { \
            break; \
        } \
        *bits_ptr = (*bits_ptr << 8) | *src_ptr++; \
        remain_bit += 8; \
    } \
    d = *bits_ptr;

#define BIT_SET(dest_ptr, ch) \
    *dest_ptr++ = ch;

static inline unsigned short swapBigShortToHost(unsigned short val)
{
    return ((val & 0xFF) << 8) | ((val >> 8) & 0xFF);
}

QImage TYGetSpbWithData(const QByteArray& d, size_t encode_size, size_t decode_size)
{
    const unsigned char* data = reinterpret_cast<const unsigned char*>(d.constData());
    unsigned char* src;
    unsigned char* dest;
    unsigned char* tmp_data;
    unsigned char* p;
    unsigned char* q;
    size_t tmp_data_size;
    unsigned int bits;
    int nbit;
    int mask;
    int remain_bit;
    int t;
    unsigned char ch;
    int rgb;
    int i;
    int j;
    unsigned char* data_bmp;
    unsigned int data_size;
    int width;
    int height;
    QImage bitmap;
    
    width = swapBigShortToHost(*(unsigned short*)data); //get_mem_be16(&data[0]);
    height = swapBigShortToHost(*(unsigned short*)(data + 2)); //get_mem_be16(&data[2]);
    //img->bpp = 8 * BPP;

    assert(width > 0); //_ASSERT(img->width > 0);
    assert(height > 0); //_ASSERT(img->height > 0);
    
    data_size = width * height * BPP;
    data_bmp = new unsigned char[data_size];
    
    tmp_data_size = width * height;
    tmp_data = new unsigned char[tmp_data_size + 4];
    
    src = const_cast<unsigned char*>(&data[4]);
    dest = tmp_data;
    bits = 0;
    remain_bit = 0;
    for (rgb = 2; rgb >= 0; rgb--, dest = tmp_data) {
        BIT_GET8(ch, &bits, src, remain_bit, encode_size);
        BIT_SET(dest, ch);
        
        while (dest < tmp_data + tmp_data_size) {
            BIT_GETN(nbit, 3, &bits, src, remain_bit, encode_size);
            if (nbit == 0) {
                BIT_SET(dest, ch);
                BIT_SET(dest, ch);
                BIT_SET(dest, ch);
                BIT_SET(dest, ch);
                continue;
            }

            if (nbit == 7) {
                BIT_GETN(mask, 1, &bits, src, remain_bit, encode_size);
                mask++;
            } else {
                mask = nbit + 2;
            }

            for (i = 0; i < 4; i++) {
                if (mask == 8) {
                    BIT_GET8(ch, &bits, src, remain_bit, encode_size);
                } else {
                    BIT_GETN(t, mask, &bits, src, remain_bit, encode_size);
                    if (t & 1) {
                        ch += (t >> 1) + 1;
                    } else {
                        ch -= (t >> 1);
                    }
                }
                BIT_SET(dest, ch);
            }
        }
        
        p = tmp_data;
        q = data_bmp + rgb;
        for (j = 0; j < height / 2; j++) {
            for (i = 0; i < width; i++) {
                *q = *p++;
                q += 3;
            }
            q += width * 3;
            for (i = 0; i < width; i++) {
                q -= 3;
                *q = *p++;
            }
            q += width * 3;
        }
        if (height & 1) {
            for (i = 0; i < width; i++) {
                *q = *p++;
                q += 3;
            }
        }
    }

    // オブジェクトの作成とデータ転送
    bitmap = QImage(data_bmp, width, height, QImage::Format_RGB888);
    
    // Copy data (QImage takes ownership or makes copy depending on usage)
    QImage result = bitmap.copy();
    
    delete[] data_bmp;
    delete[] tmp_data;
    
    return result;
}
