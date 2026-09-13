//
//  TYWaveBooster.m
//  Tukuyomi
//
//  Created by toveta on Wed Jul 03 2002.
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

#include "TYWaveBooster.h"
#include <QByteArray>
#include <cstring>
#include <cstdint>

QByteArray TYWaveBoost(const QByteArray& sourceData, int volume)
{
    if (sourceData.isEmpty())
        return QByteArray();

    QByteArray copyData = sourceData;
    unsigned char* ptr = reinterpret_cast<unsigned char*>(copyData.data());

    // RIFFヘッダチェック
    if (std::strncmp(reinterpret_cast<const char*>(ptr), "RIFF", 4) != 0) { return sourceData; }
    ptr+=8;

    // WAVEヘッダチェック
    if (std::strncmp(reinterpret_cast<const char*>(ptr), "WAVE", 4) != 0) { return sourceData; }
    ptr+=4;

    // fmtチャンク
    ptr+=8;
    // フォーマットIDのチェック
    uint16_t format = ptr[0] | (ptr[1] << 8);
    if (format != 1) {
        qWarning() << "Not supported type WAVE File (supported only linear PCM)";
        return sourceData;
    }
    ptr+=14;

    // bitサイズ取得
    int bytesOfData = (ptr[0] | (ptr[1] << 8)) / 8;
    ptr += 2;

    // dataヘッダチェック(ヘッダ拡張部サイズやfactチャンクがあれば飛ばす)
    if (std::strncmp(reinterpret_cast<const char*>(ptr), "data", 4) == 0) {
        ptr += 4;
    } else if (std::strncmp(reinterpret_cast<const char*>(ptr), "fact", 4) == 0) {
        ptr += 16;
    } else {
        uint16_t extraSize = ptr[0] | (ptr[1] << 8);
        ptr += extraSize + 2;
        if (std::strncmp(reinterpret_cast<const char*>(ptr), "data", 4) == 0) {
            ptr += 4;
        } else if (std::strncmp(reinterpret_cast<const char*>(ptr), "fact", 4) == 0) {
            ptr += 16;
        }
    }

    
    // dataサイズ取得
    int dataSize = ptr[0] | (ptr[1] << 8) | (ptr[2] << 16) | (ptr[3] << 24);
    ptr += 4;

    if (bytesOfData == 1) {
        for (int s = 0; s < dataSize; s++, ptr++) {
            *ptr = (((*ptr - 128) * volume) >> 7) + 128;
        }
    } else {
        for (int s = 0; s < dataSize; s += 2, ptr += 2) {
            // Swap処理
            unsigned char temp = ptr[0];
            ptr[0] = ptr[1];
            ptr[1] = temp;

            // 振幅の減算処理
            int16_t* sample = reinterpret_cast<int16_t*>(ptr);
            *sample = (*sample * volume) / 256;

            // Swap処理
            temp = ptr[0];
            ptr[0] = ptr[1];
            ptr[1] = temp;
        }
    }
    
    return copyData;
}
