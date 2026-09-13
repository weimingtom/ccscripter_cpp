//
//  TYWave2Aiff.m
//  Tukuyomi
//
//  Created by toveta on Sun Aug 25 2002.
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

#include <QByteArray>
#include <QCoreApplication>
#include <QDataStream>
#include <QDebug>
#include <QDir>
#include <QPluginLoader>
#include <QtEndian>

#include "TYWave2Aiff.h"
#include "TYMiscUtil.h"
#include "TYWaveDecorder.h"
#include "TYEnviroment.h"

#define AIFF_HEADER_SIZE 54

static const unsigned char SAMPLE_RATE_22050[] = { 0x40, 0x0d, 0xac, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const unsigned char SAMPLE_RATE_44100[] = { 0x40, 0x0e, 0xac, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

#define WAVE_FORMAT_PCM 1
static QDir pluginBundle;

static inline unsigned short swapShort(unsigned short val) {
    return ((val & 0x00FF) << 8) | ((val & 0xFF00) >> 8);
}

static inline unsigned int swapInt(unsigned int val) {
    return ((val & 0x000000FF) << 24) |
           ((val & 0x0000FF00) << 8)  |
           ((val & 0x00FF0000) >> 8)  |
           ((val & 0xFF000000) >> 24);
}

static bool loadExtra(void);

static const QString OGG_DECODER_NAME = QString::fromLatin1("OggDecoder.bundle");

QByteArray TYWave2Aiff(const QByteArray& sourceData, int volume)
{
    QByteArray convData;
    const unsigned char *ptr;
    unsigned char *cnvptr;
    int bytesOfData;
    int dataSize,s;
    int channel,block_align,rate;
    bool extendData = false;
    int format,frame;
    int swapData=0;
    TYWaveDecorder* plugin = nullptr;

    if (sourceData.isEmpty())
        return QByteArray();
    
    ptr = reinterpret_cast<const unsigned char*>(sourceData.constData());

    // RIFFヘッダもしくはOggヘッダチェック
    if (strncmp(reinterpret_cast<const char*>(ptr), "OggS", 4) == 0) {
        static bool loadOggDecoder = false;
        static QString oggDecoderPath;

        if (!loadOggDecoder) {
            QString bundlePath = getAppDirectory() + QDir::separator() + OGG_DECODER_NAME;
            QPluginLoader loader(bundlePath);
            if (loader.load()) {
                qDebug() << "loading OggDecoder";
                oggDecoderPath = bundlePath;
            } else {
                qWarning() << "can't load ogg plugin:" << loader.errorString();
            }
            loadOggDecoder = true;
        }

        if (oggDecoderPath.isEmpty()) {
            return sourceData;
        }
        
        throw "Ogg decoder not fully implemented in Qt port";
        return sourceData;
    } else {
        if (strncmp(reinterpret_cast<const char*>(ptr), "RIFF", 4) == 0) {
            Q_ASSERT(strncmp(reinterpret_cast<const char*>(ptr), "RIFF", 4) == 0);
            ptr+=8;
        
            // WAVEヘッダチェック
            Q_ASSERT(strncmp(reinterpret_cast<const char*>(ptr), "WAVE", 4) == 0);
            ptr+=4;
        
            // fmtチャンク
            ptr+=8;
            // フォーマットIDのチェック
            
            format = qFromLittleEndian<quint16>(*(reinterpret_cast<const quint16*>(ptr)));
            ptr+=2;
        } else {
            Q_ASSERT_X(false, "TYWave2Aiff", "illegal file header(RIFF or ogg)");
            return QByteArray();
        }
        switch (format) {
        case WAVE_FORMAT_PCM:
            // チャンネル数の取得
            channel = qFromLittleEndian<quint16>(*(reinterpret_cast<const quint16*>(ptr)));
            ptr+=2;
            
            // サンプルレートの取得
            rate = qFromLittleEndian<quint32>(*(reinterpret_cast<const quint32*>(ptr)));
            if( (rate != 22050) && (rate != 44100) && (rate != 11025)){
                qWarning() << "not supported sampling rate(supported only 22.05Khz or 44.1Khz or 11.025Khz)";
                return sourceData;
            }
            if(rate == 11025)
                extendData = true;
            ptr+=8;
        
            // block align(1frameあたりのバイト数) の取得
            block_align = qFromLittleEndian<quint16>(*(reinterpret_cast<const quint16*>(ptr)));
            ptr+=2;
            
            // bitサイズ取得
            bytesOfData = qFromLittleEndian<quint16>(*(reinterpret_cast<const quint16*>(ptr))) / 8;
            if(bytesOfData == 1)
                block_align *= 2;
            ptr +=2;
        
            // dataヘッダチェック(ヘッダ拡張部サイズやfactチャンクがあれば飛ばす)
            if (strncmp(reinterpret_cast<const char*>(ptr), "data", 4) == 0) {
                ptr+=4;
            } else if (strncmp(reinterpret_cast<const char*>(ptr), "fact", 4) == 0) {
                ptr += 16;
            } else {
                // リニアPCMのヘッダ拡張部はヘッダ拡張部サイズにかかわらず0?
                ptr+=2;
                //ptr+=NSSwapLittleShortToHost(*(unsigned short *)ptr) +2;
                if (strncmp(reinterpret_cast<const char*>(ptr), "data", 4) == 0) {
                    ptr+=4;
                } else if (strncmp(reinterpret_cast<const char*>(ptr), "fact", 4) == 0) {
                    ptr+=16;
                } else {
                    qWarning() << "this wave data have extra header size, and can't find data or fact chunk.";
                    return sourceData;
                }
            }
            
            // dataサイズ取得、変換。8bitおよび11.025Khzならそれぞれ2倍
            dataSize = qFromLittleEndian<quint32>(*(reinterpret_cast<const quint32*>(ptr)));
            dataSize *= (extendData ? 2 : 1 ) * ((bytesOfData == 1) ? 2 : 1);
            ptr +=4;
    
            frame = dataSize / block_align;
            swapData=1;
            
            break;
        default:
            {
                if (!loadExtra())
                    return sourceData;

                throw "Plugin-based format decoding not fully implemented";
                return sourceData;
            }
        }
    }


    // AIFFデータのヘッダ作成
    convData.resize(dataSize + AIFF_HEADER_SIZE);
    cnvptr = reinterpret_cast<unsigned char*>(convData.data());
    memcpy(cnvptr,"FORM",4); cnvptr+=4;
    *(unsigned int*)cnvptr = swapInt(dataSize + AIFF_HEADER_SIZE - 8); cnvptr += 4;
    memcpy(cnvptr,"AIFF",4); cnvptr+=4;
    memcpy(cnvptr,"COMM",4); cnvptr+=4;
    *(unsigned int*)cnvptr = swapInt(18); cnvptr += 4;
    *(unsigned short*)cnvptr = swapShort(channel); cnvptr += 2;
    *(unsigned int*)cnvptr = swapInt(frame); cnvptr += 4;
    *(unsigned short*)cnvptr = swapShort(16); cnvptr += 2; // bits of sample
    memcpy(cnvptr, (rate == 44100) ? SAMPLE_RATE_44100 : SAMPLE_RATE_22050, 10); cnvptr += 10;
    memcpy(cnvptr,"SSND",4); cnvptr+=4;
    *(unsigned int*)cnvptr = swapInt(dataSize + 8); cnvptr += 4;
    memset(cnvptr,'\0',4); cnvptr+=4;
    memset(cnvptr,'\0',4); cnvptr+=4;    

    // データ部に格納(ついでにヴォリュームの変換処理)
    if(!extendData){
        // 22.05Khz or 44.1Khz
        if(bytesOfData==1){
            for(s=0; s < dataSize; s+=2,ptr++,cnvptr+=2){
                *(short*)cnvptr = swapShort((*ptr - 128) * volume);
            }
        } else {
            if(dataSize % 2){
                qWarning() << "ileagal data size wave data";
                return sourceData;
            }

            if(swapData){
                for(s=0; s < dataSize; s+=2,ptr+=2,cnvptr+=2){
                    *(short*)cnvptr = swapShort((short)qFromLittleEndian<quint16>(*(unsigned short*)ptr)) * volume / 256;
                }
            } else {
                for(s=0; s < dataSize; s+=2,ptr+=2,cnvptr+=2){
                    *(short*)cnvptr = (*(short *)ptr) *volume /256;
                }
            }
        }
    } else {
        // 11.025Khz
        if(bytesOfData==1){
            // 8bit
            if(channel==1){
                // mono
                if(dataSize == 0){
                    qWarning() << "ileagal data size wave data";
                    return sourceData;
                }
                *(short*)cnvptr = swapShort((*ptr - 128) * volume);
                ptr++; cnvptr+=4;                
                for(s=4; s < dataSize; s+=4,ptr++,cnvptr+=4){
                    *(short*)cnvptr = swapShort((*ptr - 128) * volume);
                    *(((short*)cnvptr)-1) = *(short*)cnvptr -(*(short*)cnvptr -*(((short*)cnvptr)-2)) / 2;
                }
            } else {
                // stereo
                if(dataSize % 2){
                    qWarning() << "ileagal data size wave data";
                    return sourceData;
                }
            
                *(short*)cnvptr = swapShort((*ptr - 128) * volume);
                *(((short*)cnvptr) + 1) = swapShort((*(ptr + 1) - 128) * volume);
                ptr+=2,cnvptr+=8;
                for(s=8; s < dataSize; s+=8,ptr+=2,cnvptr+=8){
                    // L
                    *(short*)cnvptr = swapShort((*ptr - 128) * volume);
                    *(((short*)cnvptr)-2) = *(short*)cnvptr -(*(short*)cnvptr -*(((short*)cnvptr)-4)) / 2;
                    // R
                    *(((short*)cnvptr) + 1) = swapShort((*(ptr + 1) - 128) * volume);
                    *(((short*)cnvptr)-1) = *(((short*)cnvptr)+1) -(*(((short*)cnvptr)+1) -*(((short*)cnvptr)-3)) / 2;
                }
            }
        } else {
            // 16bit
            if(channel==1){
                // mono
                if(dataSize % 2){
                    qWarning() << "ileagal data size wave data";
                    return sourceData;
                }

                *(short*)cnvptr = qFromLittleEndian<quint16>(*(unsigned short*)ptr);
                *(short*)cnvptr = *(short*)cnvptr *volume / 256;
                ptr+=2; cnvptr+=4;
                if(swapData){            
                    for(s=4; s < dataSize; s+=4,ptr+=2,cnvptr+=4){
                        *(short*)cnvptr = qFromLittleEndian<quint16>(*(unsigned short*)ptr);
                        *(short*)cnvptr = *(short*)cnvptr *volume / 256;
                        *(((short*)cnvptr)-1) = *(short*)cnvptr -(*(short*)cnvptr -*(((short*)cnvptr)-2)) / 2;
                    }
                } else {
                    for(s=4; s < dataSize; s+=4,ptr+=2,cnvptr+=4){
                        *(short*)cnvptr = *(short*)cnvptr *volume / 256;
                        *(((short*)cnvptr)-1) = *(short*)cnvptr -(*(short*)cnvptr -*(((short*)cnvptr)-2)) / 2;
                    }
                }
            } else {
                // stereo
                if(dataSize % 4){
                    qWarning() << "ileagal data size wave data";
                    return sourceData;
                }

                *(short*)cnvptr = qFromLittleEndian<quint16>(*(unsigned short*)ptr);
                *(short*)cnvptr = *(short*)cnvptr *volume / 256;
                *(((short*)cnvptr) + 1) = qFromLittleEndian<quint16>(*((unsigned short*)ptr + 1));
                *(((short*)cnvptr)+1) = *(((short*)cnvptr)+1) *volume / 256;
                ptr+=4; cnvptr+=8;
                if(swapData){
                    for(s=8; s < dataSize; s+=8,ptr+=4,cnvptr+=8){
                        // L
                        *(short*)cnvptr = qFromLittleEndian<quint16>(*(unsigned short*)ptr);
                        *(short*)cnvptr = *(short*)cnvptr *volume / 256;
                        *(((short*)cnvptr)-2) = *(short*)cnvptr -(*(short*)cnvptr -*(((short*)cnvptr)-4)) / 2;
                        // R
                        *(((short*)cnvptr) + 1) = qFromLittleEndian<quint16>(*((unsigned short*)ptr + 1));
                        *(((short*)cnvptr)+1) = *(((short*)cnvptr)+1) *volume / 256;
                        *(((short*)cnvptr)-1) = *(((short*)cnvptr)+1) -(*(((short*)cnvptr)+1) -*(((short*)cnvptr)-3)) / 2; 
                    }
                } else {
                    for(s=8; s < dataSize; s+=8,ptr+=4,cnvptr+=8){
                        // L
                        *(short*)cnvptr = *(short*)cnvptr *volume / 256;
                        *(((short*)cnvptr)-2) = *(short*)cnvptr -(*(short*)cnvptr -*(((short*)cnvptr)-4)) / 2;
                        // R
                        *(((short*)cnvptr)+1) = *(((short*)cnvptr)+1) *volume / 256;
                        *(((short*)cnvptr)-1) = *(((short*)cnvptr)+1) -(*(((short*)cnvptr)+1) -*(((short*)cnvptr)-3)) / 2; 
                    }
                }
            }
        }
    }
    
    if(plugin)
        delete plugin;

    return convData;
}

static bool loadExtra(void)
{
    static bool initialized = false, loaded = false;
    
    if(initialized && !loaded) {
        return false;
    } else if(!initialized) {
        QString path = getAppDirectory() + QDir::separator() +
                       TYEnviroment::objectForKey(TYWaveExtraNameEnviroment) + QStringLiteral(".bundle");

        initialized = true;

        pluginBundle = QDir(path);
        if (!pluginBundle.exists()) {
            qWarning() << "can't load plugin:" << path;
            return false;
            }

        QPluginLoader loader(path);
        if (!loader.load()) {
            qWarning() << "can't load plugin:" << loader.errorString();
            loaded = true;
            return false;
        }
        qDebug() << "loading WaveExtra";
        loaded = true;
    }
    
    return true;
}

void TYWave2AiffCleanUp(void) // call it on quit Application
{
}

QByteArray TYVerifyAiff(const QByteArray& data)
{
    const char *aiffPtr;
    
    aiffPtr = data.constData();
    
    Q_ASSERT_X(data.size() > 46, "TYVerifyAiff", "not enought data size");
    
    Q_ASSERT_X(strncmp(aiffPtr, "FORM", 4) == 0, "TYVerifyAiff", "not exists FORM Header");
    Q_ASSERT_X(*(unsigned int*)(aiffPtr + 4) == (unsigned int)(data.size() - 8), "TYVerifyAiff", "no match file size");
    Q_ASSERT_X(strncmp(aiffPtr + 8, "AIFF", 4) == 0, "TYVerifyAiff", "not exists AIFF Header");
    
    Q_ASSERT_X(strncmp(aiffPtr + 12, "COMM", 4) == 0, "TYVerifyAiff", "not exists COMM Header");
    Q_ASSERT_X(*(unsigned int*)(aiffPtr + 16) == 18, "TYVerifyAiff", "no match comm size");
    Q_ASSERT_X((*(unsigned short*)(aiffPtr + 20) == 1) || (*(unsigned short*)(aiffPtr + 20) == 2), "TYVerifyAiff", "NSSound support only mono or streo");
        
    Q_ASSERT_X(qFromBigEndian<quint16>(*((unsigned short*)(aiffPtr + 26))) == 16, "TYVerifyAiff", "NSSound support only 16-bits");
    
    Q_ASSERT_X((memcmp(aiffPtr + 28, SAMPLE_RATE_44100, 10) == 0) ||
               (memcmp(aiffPtr + 28, SAMPLE_RATE_22050, 10) == 0), "TYVerifyAiff", "not sampleRate 44100 or 22500");

    Q_ASSERT_X(strncmp(aiffPtr + 38, "SSND", 4) == 0, "TYVerifyAiff", "not exists SSND Header");
    Q_ASSERT_X((qFromBigEndian<quint32>(*(unsigned int*)(aiffPtr + 42)) - 8 == qFromBigEndian<quint16>(*((unsigned short*)(aiffPtr + 20))) * qFromBigEndian<quint32>(*((unsigned int*)(aiffPtr + 22))) * 2),
               "TYVerifyAiff", "no match chunk size");
    Q_ASSERT_X(qFromBigEndian<quint32>(*(unsigned int*)(aiffPtr + 46)) == 0, "TYVerifyAiff", "illegal offset size");
    Q_ASSERT_X(qFromBigEndian<quint32>(*(unsigned int*)(aiffPtr + 50)) == 0, "TYVerifyAiff", "illegal block size");
    
    Q_ASSERT_X((qFromBigEndian<quint32>(*(unsigned int*)(aiffPtr + 42)) + 46 == (unsigned int)data.size()), "TYVerifyAiff", "no mutch file size to chunkSize");
    
    return data;
}
