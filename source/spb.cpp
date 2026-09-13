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

#import "spb.h"

#define BPP 3

#define	BIT_GETN(d,n)					\
if (remain_bit < n) {					\
    if (src >= data + encode_size) {			\
        break;						\
    }							\
    bits = (bits << 8) | *src++;			\
        remain_bit += 8;				\
}							\
remain_bit -= n;					\
d = (bits >> remain_bit) & ((1 << n) - 1);

#define	BIT_GET8(d)					\
if (remain_bit < 8) {					\
    if (src >= data + encode_size) {			\
        break;						\
    }							\
    bits = (bits << 8) | *src++;			\
}							\
d = (bits >> remain_bit);

#define	BIT_SET()					\
*dest++ = ch;

NSBitmapImageRep* TYGetSpbWithData(NSData* d,size_t encode_size,size_t decode_size)
{
    const unsigned char *data=[ d bytes ];
    unsigned char *src, *dest, *tmp_data;
    unsigned char *p, *q;
    size_t tmp_data_size;
    unsigned int bits;
    int nbit, mask, remain_bit;
    int t;
    unsigned char ch;
    int rgb = 2;
    int i, j;
    unsigned char *data_bmp;
    unsigned int data_size;
    int width,height;
    NSBitmapImageRep *bitmap;

    width = NSSwapBigShortToHost(*(unsigned short*)data); //get_mem_be16(&data[0]);
    height = NSSwapBigShortToHost(*(unsigned short*)(data+2)); //get_mem_be16(&data[2]);
    //img->bpp = 8 * BPP;

    NSCAssert((width > 0),@"width is ileagal"); //_ASSERT(img->width > 0);
    NSCAssert((height > 0),@"height is ileagal"); //_ASSERT(img->height > 0);

    data_size = width * height * BPP;
    data_bmp = [ [ NSMutableData dataWithLength:data_size ] mutableBytes ]; //img->data = (unsigned char *)Emalloc(img->data_size);

    tmp_data_size = width * height;
    tmp_data = [ [ NSMutableData dataWithLength:tmp_data_size +4 ] mutableBytes ]; //(unsigned char *)Emalloc(tmp_data_size);

    src = (unsigned char*)(&data[4]);
    dest = tmp_data;
    bits = 0;
    remain_bit = 0;
    for (rgb = 2; rgb >= 0; rgb--, dest = tmp_data) {
        BIT_GET8(ch);
        BIT_SET();

        while (dest < tmp_data + tmp_data_size) {
            BIT_GETN(nbit, 3);
            if (nbit == 0) {
                BIT_SET();
                BIT_SET();
                BIT_SET();
                BIT_SET();
                continue;
            }

            if (nbit == 7) {
                BIT_GETN(mask, 1);
                mask++;
            } else {
                mask = nbit + 2;
            }

            for (i = 0; i < 4; i++) {
                if (mask == 8) {
                    BIT_GET8(ch);
                } else {
                    BIT_GETN(t, mask);
                    if (t & 1) {
                        ch += (t >> 1) + 1;
                    } else {
                        ch -= (t >> 1);
                    }
                }
                BIT_SET();
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
    bitmap = [ [ NSBitmapImageRep alloc ] initWithBitmapDataPlanes:NULL
                                                        pixelsWide:width
                                                        pixelsHigh:height
                                                     bitsPerSample:8
                                                   samplesPerPixel:3
                                                          hasAlpha:NO
                                                          isPlanar:NO
                                                    colorSpaceName:NSCalibratedRGBColorSpace
                                                       bytesPerRow:0
                                                      bitsPerPixel:0 ];
    memcpy([ bitmap bitmapData ],data_bmp,width * height * 3);

    return [ bitmap autorelease ];
    
}
