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

#import "TYWave2Aiff.h"
#import "TYMiscUtil.h"
#import "TYWaveDecorder.h"
#import "TYEnviroment.h"

#define AIFF_HEADER_SIZE 54

static const char SAMPLE_RATE_22050[] = { 0x40,0x0d,0xac,0x44,0x00,0x00,0x00,0x00,0x00,0x00 };
static const char SAMPLE_RATE_44100[] = { 0x40,0x0e,0xac,0x44,0x00,0x00,0x00,0x00,0x00,0x00 };

#define WAVE_FORMAT_PCM 1
static NSBundle *pluginBundle;

static BOOL loadExtra(void);

static NSString* const OGG_DECODER_NAME = @"OggDecoder.bundle";

NSData* TYWave2Aiff(NSData* sourceData,int volume)
{
    NSMutableData* convData;
    const unsigned char *ptr;
    unsigned char *cnvptr;
    int bytesOfData;
    int dataSize,s;
    int channel,block_align,rate;
    BOOL extendData=NO;
    int format,frame;
    int swapData=0;
    NSObject <TYWaveDecorder> *plugin = nil;

    if(!sourceData)
        return nil;
    
    ptr = [ sourceData bytes ];

    // RIFFヘッダもしくはOggヘッダチェック
    if (strncmp(ptr, "OggS", 4) == 0) {
        static BOOL loadOggDecoder = NO;
        static Class oggDecoderClass = Nil;
        
        id<TYWaveDecorder> decoder;
        
        if (loadOggDecoder == NO) {
            NSBundle *bundle;
            bundle = [ [NSBundle bundleWithPath:
                [ getAppDirectory() stringByAppendingPathComponent:OGG_DECODER_NAME ] ] retain ];
            if((oggDecoderClass = [ bundle principalClass ])){
                if(![ oggDecoderClass conformsToProtocol:@protocol(TYWaveDecorder) ]){
                    NSLog(@"invalid plugin it isn't conformsProtocol");
                    oggDecoderClass = Nil;
                }
            } else {
                NSLog(@"can't load ogg plugin");
                oggDecoderClass = Nil;
            }
            NSLog(@"loading OggDecoder");
            loadOggDecoder = YES;
        }
        
        if (oggDecoderClass == Nil) {
            return sourceData;
        }
        
        decoder = [ [ oggDecoderClass alloc ] initWithData:sourceData ];

        if ([ decoder getSamples:(short**)(&ptr)
                         frames:&frame
                       channels:&channel
                           rate:&rate ] == NO) {
            return sourceData;
        }
        
        if(rate == 11025)
            extendData = YES;
            
        dataSize = frame * 2 * channel * (extendData ? 2 : 1 );
        bytesOfData=2;
    } else {
        if(strncmp(ptr,"RIFF",4)==0) {
            NSCAssert((strncmp(ptr,"RIFF",4)==0),@"ileagal RIFF header");
            ptr+=8;
        
            // WAVEヘッダチェック
            NSCAssert((strncmp(ptr,"WAVE",4)==0),@"this data is not WAVE FILE");
            ptr+=4;
        
            // fmtチャンク
            ptr+=8;
            // フォーマットIDのチェック
            
            format = NSSwapLittleShortToHost(*(unsigned short *)ptr);
            ptr+=2;
        } else {
            NSCAssert((0),@"illegal file header(RIFF or ogg)");
            return nil;
        }
        switch (format) {
        case WAVE_FORMAT_PCM:
            // チャンネル数の取得
            channel = NSSwapLittleShortToHost(*(unsigned short *)ptr);
            ptr+=2;
            
            // サンプルレートの取得
            rate = NSSwapLittleIntToHost(*(unsigned int *)ptr);
            if( (rate != 22050) && (rate != 44100) && (rate != 11025)){
                NSLog(@"not supported sampling rate(supported only 22.05Khz or 44.1Khz or 11.025Khz)");
                return sourceData;
            }
            if(rate == 11025)
                extendData = YES;
            ptr+=8;
        
            // block align(1frameあたりのバイト数) の取得
            block_align = NSSwapLittleShortToHost(*(unsigned short *)ptr);
            ptr+=2;
            
            // bitサイズ取得
            bytesOfData=NSSwapLittleShortToHost(*(unsigned short *)ptr) / 8;
            if(bytesOfData == 1)
                block_align *= 2;
            ptr +=2;
        
            // dataヘッダチェック(ヘッダ拡張部サイズやfactチャンクがあれば飛ばす)
            if(strncmp(ptr,"data",4)==0){
                ptr+=4;
            } else if(strncmp(ptr,"fact",4)==0){
                ptr+=16;
            } else {
                // リニアPCMのヘッダ拡張部はヘッダ拡張部サイズにかかわらず0?
                ptr+=2;
                //ptr+=NSSwapLittleShortToHost(*(unsigned short *)ptr) +2;
                if(strncmp(ptr,"data",4)==0){
                    ptr+=4;
                } else if(strncmp(ptr,"fact",4)==0){
                    ptr+=16;
                } else {
                    NSLog(@"this wave data have extra header size, and can't find data or fact chunk.");
                    return sourceData;
                }
            }
            
            // dataサイズ取得、変換。8bitおよび11.025Khzならそれぞれ2倍
            dataSize=NSSwapLittleIntToHost(*(unsigned int *)ptr);
            dataSize *= (extendData ? 2 : 1 ) * ((bytesOfData == 1) ? 2 : 1);
            ptr +=4;
    
            frame = dataSize / block_align;
            swapData=1;
            
            break;
        default:
            {
                Class class;
                
                if(!loadExtra())
                    return sourceData;
    
                class = [ pluginBundle principalClass ];
                plugin = [ class alloc ];
                plugin = [ plugin initWithData:sourceData ];
                //plugin = [ plugin autorelease ];
                
                if(![ plugin getSamples:(short**)(&ptr)
                            frames:&frame
                        channels:&channel
                            rate:&rate ])
                    return sourceData;
                
                if(rate == 11025)
                    extendData = YES;
                    
                dataSize = frame * 2 * channel * (extendData ? 2 : 1 );
                bytesOfData=2;
            }
        }
    }


    // AIFFデータのヘッダ作成
    convData = [ NSMutableData dataWithLength:dataSize +AIFF_HEADER_SIZE ];
    cnvptr = [ convData mutableBytes ];
    memcpy(cnvptr,"FORM",4); cnvptr+=4;
    *(unsigned int*)cnvptr=NSSwapHostIntToBig(dataSize +AIFF_HEADER_SIZE -8); cnvptr+=4;
    memcpy(cnvptr,"AIFF",4); cnvptr+=4;
    memcpy(cnvptr,"COMM",4); cnvptr+=4;
    *(unsigned int*)cnvptr=NSSwapHostIntToBig(18); cnvptr+=4;    
    *(unsigned short*)cnvptr=NSSwapHostShortToBig(channel); cnvptr+=2;
    *(unsigned int*)cnvptr=NSSwapHostIntToBig(frame); cnvptr+=4;
    *(unsigned short*)cnvptr=NSSwapHostShortToBig(16); cnvptr+=2; // bits of sample
    memcpy(cnvptr,((rate==44100) ? SAMPLE_RATE_44100 : SAMPLE_RATE_22050),10); cnvptr+=10;
    memcpy(cnvptr,"SSND",4); cnvptr+=4;
    *(unsigned int*)cnvptr=NSSwapHostIntToBig(dataSize + 8); cnvptr+=4;
    memset(cnvptr,'\0',4); cnvptr+=4;
    memset(cnvptr,'\0',4); cnvptr+=4;    

    // データ部に格納(ついでにヴォリュームの変換処理)
    if(!extendData){
        // 22.05Khz or 44.1Khz
        if(bytesOfData==1){
            for(s=0; s < dataSize; s+=2,ptr++,cnvptr+=2){
                *(short*)cnvptr = NSSwapHostShortToBig((*ptr -128) *volume);
            }
        } else {
            if(dataSize % 2){
                NSLog(@"ileagal data size wave data");
                return sourceData;
            }

            if(swapData){
                for(s=0; s < dataSize; s+=2,ptr+=2,cnvptr+=2){
                    *(short*)cnvptr = ((short)NSSwapShort(*(unsigned short *)ptr)) *volume /256;
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
                    NSLog(@"ileagal data size wave data");
                    return sourceData;
                }
                *(short*)cnvptr = NSSwapHostShortToBig((*ptr -128) *volume);
                ptr++; cnvptr+=4;                
                for(s=4; s < dataSize; s+=4,ptr++,cnvptr+=4){
                    *(short*)cnvptr = NSSwapHostShortToBig((*ptr -128) *volume);
                    *(((short*)cnvptr)-1) = *(short*)cnvptr -(*(short*)cnvptr -*(((short*)cnvptr)-2)) / 2;
                }
            } else {
                // stereo
                if(dataSize % 2){
                    NSLog(@"ileagal data size wave data");
                    return sourceData;
                }
            
                *(short*)cnvptr = NSSwapHostShortToBig((*ptr -128) *volume);
                *(((short*)cnvptr)+1) = NSSwapHostShortToBig((*(ptr+1) -128) *volume);
                ptr+=2,cnvptr+=8;
                for(s=8; s < dataSize; s+=8,ptr+=2,cnvptr+=8){
                    // L
                    *(short*)cnvptr = NSSwapHostShortToBig((*ptr -128) *volume);
                    *(((short*)cnvptr)-2) = *(short*)cnvptr -(*(short*)cnvptr -*(((short*)cnvptr)-4)) / 2;
                    // R
                    *(((short*)cnvptr)+1) = NSSwapHostShortToBig((*(ptr+1) -128) *volume);
                    *(((short*)cnvptr)-1) = *(((short*)cnvptr)+1) -(*(((short*)cnvptr)+1) -*(((short*)cnvptr)-3)) / 2;
                }
            }
        } else {
            // 16bit
            if(channel==1){
                // mono
                if(dataSize % 2){
                    NSLog(@"ileagal data size wave data");
                    return sourceData;
                }

                *(short*)cnvptr = NSSwapShort(*(unsigned short *)ptr);
                *(short*)cnvptr = *(short*)cnvptr *volume / 256;
                ptr+=2; cnvptr+=4;
                if(swapData){            
                    for(s=4; s < dataSize; s+=4,ptr+=2,cnvptr+=4){
                        *(short*)cnvptr = NSSwapShort(*(unsigned short *)ptr);
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
                    NSLog(@"ileagal data size wave data");
                    return sourceData;
                }

                *(short*)cnvptr = NSSwapShort(*(unsigned short *)ptr);
                *(short*)cnvptr = *(short*)cnvptr *volume / 256;
                *(((short*)cnvptr)+1) = NSSwapShort(*((unsigned short *)ptr+1));
                *(((short*)cnvptr)+1) = *(((short*)cnvptr)+1) *volume / 256;
                ptr+=4; cnvptr+=8;
                if(swapData){
                    for(s=8; s < dataSize; s+=8,ptr+=4,cnvptr+=8){
                        // L
                        *(short*)cnvptr = NSSwapShort(*(unsigned short *)ptr);
                        *(short*)cnvptr = *(short*)cnvptr *volume / 256;
                        *(((short*)cnvptr)-2) = *(short*)cnvptr -(*(short*)cnvptr -*(((short*)cnvptr)-4)) / 2;
                        // R
                        *(((short*)cnvptr)+1) = NSSwapShort(*((unsigned short *)ptr+1));
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
        [ plugin release ];
    
    return convData;
}

static BOOL loadExtra(void)
{
    static BOOL initialized,loaded;
    
    if(initialized && !loaded) {
        return NO;
    } else if(!initialized) {
        Class pluginClass;
        NSString *path=[ getAppDirectory() stringByAppendingPathComponent:[ [ TYEnviroment objectForKey:TYWaveExtraNameEnviroment ] stringByAppendingPathExtension:@"bundle" ] ];

        initialized=YES;

        pluginBundle = [ [NSBundle bundleWithPath:path] retain ];
        if((pluginClass = [ pluginBundle principalClass ])){
            if(![ pluginClass conformsToProtocol:@protocol(TYWaveDecorder) ]){
                loaded=YES;
                NSLog(@"invalid plugin it isn't conformsProtocol");
                return NO;
            }
        } else {
            NSLog(@"can't load plugin");
            return NO;
        }
        NSLog(@"loading WaveExtra");
        loaded = YES;
    }
    
    return YES;
}

void TYWave2AiffCleanUp(void) // call it on quit Application
{
}

NSData* TYVerifyAiff(NSData *data)
{
    const char *aiffPtr;
    
    aiffPtr = [ data bytes ];
    
    NSCAssert(([ data length ] > 46), @"not enought data size");
    
    NSCAssert((strncmp(aiffPtr, "FORM", 4) == 0), @"not exists FORM Header");
    NSCAssert((*(unsigned int*)(aiffPtr +4) == [ data length ] - 8), @"no match file size");
    NSCAssert((strncmp(aiffPtr + 8, "AIFF", 4) == 0), @"not exists AIFF Header");
    
    NSCAssert((strncmp(aiffPtr + 12, "COMM", 4) == 0), @"not exists COMM Header");
    NSCAssert((*(unsigned int*)(aiffPtr + 16) == 18), @"no match comm size");
    NSCAssert(((*(unsigned short*)(aiffPtr + 20) == 1) || (*(unsigned short*)(aiffPtr + 20) == 2)), @"NSSound support only mono or streo");
        
    NSCAssert((*((unsigned short*)(aiffPtr + 26)) == 16), @"NSSound support only 16-bits");
    
    NSCAssert(((memcmp(aiffPtr + 28, SAMPLE_RATE_44100, 10) == 0) ||
                (memcmp(aiffPtr + 28, SAMPLE_RATE_22050, 10) == 0)), @"not sampleRate 44100 or 22500");

    NSCAssert((strncmp(aiffPtr + 38, "SSND", 4) == 0), @"not exists SSND Header");
    NSCAssert3((*(unsigned int*)(aiffPtr + 42) - 8 == *((unsigned short*)(aiffPtr + 20)) * *((unsigned int*)(aiffPtr + 22)) * 2 ), 
                @"no match chunk size [%d] [%d] [%d]", *(unsigned int*)(aiffPtr + 42) , *((unsigned short*)(aiffPtr + 20)) , *((unsigned int*)(aiffPtr + 22)));
    NSCAssert((*(unsigned int*)(aiffPtr + 46) == 0), @"illegal offset size");
    NSCAssert((*(unsigned int*)(aiffPtr + 50) == 0), @"illegal block size");
    
    NSCAssert((*(unsigned int*)(aiffPtr + 42) + 46 == [ data length ]), @"no mutch file size to chunkSize");
    
    return data;
}
