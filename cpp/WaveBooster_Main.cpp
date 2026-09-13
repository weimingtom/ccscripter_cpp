//
//  WaveBooster_Main.m
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
 * THIS SOFTWARE IS PROVIDED BY toveta ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL toveta OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * &ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <Foundation/Foundation.h>
#include "TYWaveBooster.h"

/*
 usage.. WaveBooster volume infile outfile
 WaveBoosterはWaveサウンドファイルのボリュームを増減させたファイルを書き出します。
 ただし、増幅させた場合のオーバーフローチェックは行っていません。
 */
int main (int argc, const char * argv[])
{
    NSData *waveData;
    NSData *convData;
    int volume;

    if(argc < 4){
        printf("usage.. WaveBooster volume infile outfile\n");
    }
    waveData = [ [ NSData alloc ] initWithContentsOfFile:[ [ NSString stringWithCString:argv[2] ] stringByExpandingTildeInPath ] ];
    volume = atoi(argv[1]);

    convData = TYWaveBoost(waveData,volume);

    [ convData writeToFile:[ NSString stringWithCString:argv[3] ] atomically:NO ];

    return 0;
}
