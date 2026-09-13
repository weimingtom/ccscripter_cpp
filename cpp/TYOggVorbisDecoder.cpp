//
//  TYOggVorbisDecoder.m
//  Tukuyomi
//
//  Created by toveta on Fri Oct 03 2003.
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

#include "TYOggVorbisDecoder.h"
#include <vorbis/vorbisfile.h>
#include <QDebug>
#include <cstring>

struct TYOVFileData {
    const char* stPtr;
    int cursor;
    int length;
};

// ogg vorbis callbacks ////////
static size_t TYOVRead(void* ptr, size_t size, size_t nmemb, void* datasource)
{
    TYOVFileData *fileData = (TYOVFileData*) datasource;
    int readLen;

    if (fileData->cursor > fileData->length) {
        return 0;
    } else if (fileData->length - fileData->cursor < size * nmemb ) {
        readLen = fileData->length - fileData->cursor;
    } else {
        readLen = size * nmemb;
    }
    
    memcpy(ptr, fileData->stPtr + fileData->cursor, readLen);
    
    fileData->cursor += readLen;
    
    return readLen / size;
}

static int TYOVSeek(void* datasource, ogg_int64_t offset, int whence)
{
    // FIXME 32bitを越えるサイズは考慮してない
    int offset_i = offset & 0x00000000FFFFFFFF;
    TYOVFileData *fileData = (TYOVFileData*) datasource;
    
    switch (whence) {
    case SEEK_CUR:
        if (fileData->cursor + offset_i > fileData->length) {
            fileData->cursor = fileData->length;
        } else {
            fileData->cursor += offset_i;
        }
        break;
    case SEEK_END:
        if (offset_i > fileData->length) {
            fileData->cursor = 0;
        } else {
            fileData->cursor = fileData->length - offset_i;
        }
        break;
    case SEEK_SET:
        if (offset_i > fileData->length) {
            fileData->cursor = fileData->length;
        } else {
            fileData->cursor = offset_i;
        }
        break;
    default:
        return -1;
        break;
    }
    
    return 0;
}

static int TYOVClose(void* datasource)
{
    TYOVFileData *fileData = (TYOVFileData*) datasource;

    fileData->length = 0;

    return 0;
}

static long TYOVTell(void* datasource)
{
    TYOVFileData* fileData = (TYOVFileData*)datasource;
    return fileData->cursor;
}
////////////////////////////////

TYOggVorbisDecoder::TYOggVorbisDecoder(QObject* parent)
    : QObject(parent)
    , m_ovPtr(nullptr)
{
}

TYOggVorbisDecoder::~TYOggVorbisDecoder()
{
}

bool TYOggVorbisDecoder::initWithData(const QByteArray& sourceData)
{
    m_ovData = sourceData;
    m_ovPtr = m_ovData.constData();
    return true;
}

bool TYOggVorbisDecoder::getSamples(short** samples_ptr,
                                     int* frames,
                                     int* channels,
                                     int* rate)
{
    OggVorbis_File vf;
    ov_callbacks cb;
    TYOVFileData fileData;
    cb.read_func = TYOVRead;
    cb.seek_func = TYOVSeek;
    cb.close_func = TYOVClose;
    cb.tell_func = TYOVTell;

    // オープン処理
    {
        int ret;
        
        fileData.stPtr = m_ovPtr;
        fileData.cursor = 0;
        fileData.length = m_ovData.length();
        
        ret = ov_open_callbacks(&fileData, &vf, nullptr, 0, cb);
        
        if (ret != 0) { 
            qWarning() << "can't open ogg vorbis file!";
            ov_clear(&vf);
            return false;
        }
    }
    
    // ファイル情報の取得
    {
        vorbis_info *vi;
        
        vi = ov_info(&vf, -1);
        
        if (vi == nullptr) {
            qWarning() << "can't get ogg vorbis info!";
            ov_clear(&vf);
            return false;
        }
        
        *channels = vi->channels;
        *rate = vi->rate;
    }

    // 総サンプル数の取得
    {
        *frames = ov_pcm_total(&vf, -1);
        if (*frames == OV_EINVAL) {
            qWarning() << "ogg vorbis invalid pcm total error!";
            ov_clear(&vf);
            return false;
        }
    }
    
    // 読みこみ
    {
        static const int MAX_BUFFER_LENGTH = 4096;
        int readLength = 0;
        int dataLength = *frames * *channels * 2;
        char *readPtr;
        
        // 読み込み結果格納オブジェクト確保
        m_sampleData.resize(dataLength);
        readPtr = m_sampleData.data();
        *samples_ptr = reinterpret_cast<short*>(m_sampleData.data());
        
        while (1) {
            int ret;
            int length;
            
            // 読みこみ長の決定
            if (dataLength -readLength > MAX_BUFFER_LENGTH) {
                length = MAX_BUFFER_LENGTH;
            } else {
                length = dataLength -readLength;
                if (length == 0) {
                    break;
                }
            }
            
            // 読み込みと結果チェック
            ret = ov_read(&vf, readPtr, length, 0, 2, 1, nullptr);
            if (ret <= 0) {
                if (ret == 0) {
                    break;
                } else if (ret == OV_HOLE) {
                    qWarning() << "OV_HOLE ERROR!";
                    ov_clear(&vf);
                    return false;
                } else if (ret == OV_EBADLINK) {
                    qWarning() << "OV_EBADLINK ERROR!";
                    ov_clear(&vf);
                    return false;
                }
            }
            
            readPtr += ret;
            readLength += ret;
        }
        ov_clear(&vf);
    }
    
    return true;
}
