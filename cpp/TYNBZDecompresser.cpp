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

#include "TYNBZDecompresser.h"
#include <bzlib.h>
#include <QFile>
#include <QByteArray>
#include <cassert>
#include <cstdio>

TYNBZDecompresser* TYNBZDecompresser::m_sharedObject = nullptr;

TYNBZDecompresser::TYNBZDecompresser(QObject* parent)
    : TYDecompresser(parent)
{
}

TYNBZDecompresser::~TYNBZDecompresser()
{
}

TYNBZDecompresser* TYNBZDecompresser::sharedObject()
{
    if (!m_sharedObject) {
        m_sharedObject = new TYNBZDecompresser();
    }
    return m_sharedObject;
}

/*
 * NBZ
 *
 * +00h : DWORD(BE32) orignal size
 * +04h-: bzip -1 orignal_file
 */
QByteArray TYNBZDecompresser::decodeDataWithFileHandle(int handle, int encode_size)
{
    FILE* fp;
    unsigned int decode_size;
    unsigned char unused[BZ_MAX_UNUSED];
    BZFILE* bzf;
    unsigned char* top;
    unsigned char* p;
    QByteArray decodeData;
    unsigned char* unused_tmp;
    int bzerr;
    int nread;
    int nunused;

    QByteArray sizeData;
    decode_size = 0;
    
    fp = fdopen(handle, "r");
    if (fp == nullptr) {
        return QByteArray();
    }
    
    decodeData.resize(decode_size);
    p = top = reinterpret_cast<unsigned char*>(decodeData.data());

    nunused = 0;
    bzf = BZ2_bzReadOpen(&bzerr, fp, 0, 0, unused, nunused);
    assert((bzf != nullptr && bzerr == BZ_OK));

    while (bzerr == BZ_OK) {
        assert((p < top + decode_size));
        nread = BZ2_bzRead(&bzerr, bzf, p, 5000);
        assert(((bzerr == BZ_OK || bzerr == BZ_STREAM_END) && nread > 0));
        p += nread;
    }
    assert((bzerr == BZ_STREAM_END));

    BZ2_bzReadGetUnused(&bzerr, bzf, (void**)&unused_tmp, &nunused);
    assert((bzerr == BZ_OK));

    BZ2_bzReadClose(&bzerr, bzf);
    assert((bzerr == BZ_OK));

    //NSAssert((nunused == 0),@"nunused not equal 0");

    return decodeData;
}

void* TYNBZDecompresser::decodeBitmapWithFileHandle(int handle, int encode_size)
{
    QByteArray data = decodeDataWithFileHandle(handle, encode_size);
    return nullptr;
}
