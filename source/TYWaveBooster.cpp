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

#import "TYWaveBooster.h"

NSData* TYWaveBoost(NSData* sourceData,int volume)
{
    NSMutableData* copyData;
    unsigned char *ptr;
    int bytesOfData;
    int dataSize,s;
    unsigned char temp;

    if(!sourceData)
        return nil;
    
    copyData = [ [ sourceData mutableCopy ] autorelease ];
    ptr = [ copyData mutableBytes ];

    // RIFFヘッダチェック
    NSCAssert((strncmp(ptr,"RIFF",4)==0),@"ileagal RIFF header");
    ptr+=8;

    // WAVEヘッダチェック
    NSCAssert((strncmp(ptr,"WAVE",4)==0),@"this data is not WAVE FILE");
    ptr+=4;

    // fmtチャンク
    ptr+=8;
    // フォーマットIDのチェック
    if(NSSwapLittleShortToHost(*(unsigned short *)ptr)!=1){
        NSLog(@"not supported type WAVE File(supported only lenier PCM)");
        return sourceData;
    }
    ptr+=14;

    // bitサイズ取得
    bytesOfData=NSSwapLittleShortToHost(*(unsigned short *)ptr) / 8;
    ptr +=2;

    // dataヘッダチェック(ヘッダ拡張部サイズやfactチャンクがあれば飛ばす)
    if(strncmp(ptr,"data",4)==0){
        ptr+=4;
    } else if(strncmp(ptr,"fact",4)==0){
        ptr+=16;
    } else {
        ptr+=NSSwapLittleShortToHost(*(unsigned short *)ptr) +2;
        if(strncmp(ptr,"data",4)==0){
            ptr+=4;
        } else if(strncmp(ptr,"fact",4)==0){
            ptr+=16;
        }
    }

    
    // dataサイズ取得
    dataSize=NSSwapLittleIntToHost(*(unsigned int *)ptr);
    ptr +=4;

    if(bytesOfData==1){
        for(s=0; s < dataSize; s++,ptr++){
            *ptr = (((*ptr -128) *volume) >> 7) +128;
        }
    } else {
        for(s=0; s < dataSize; s+=2,ptr+=2){
            // Swap処理
            temp = ptr[0];
            ptr[0] = ptr[1];
            ptr[1] = temp;

            // 振幅の減算処理
            *(short *)ptr = *(short *)ptr *volume /256;

            // Swap処理
            temp = ptr[0];
            ptr[0] = ptr[1];
            ptr[1] = temp;
        }
    }
    
    return copyData;
}
