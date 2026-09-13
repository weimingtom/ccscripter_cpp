/*
 *  TYSaveDataCoding.h
 *  Tukuyomi
 *
 *  Created by toveta on Mon Dec 24 2001.
 *  Copyright (c) 2001 toveta All rights reserved.
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

#import <Foundation/Foundation.h>

@protocol TYSaveDataCoding
-(id)encodeWithSaveData;
-(void)decodeWithSaveData:(id)aObject;
@end

// constants of savedata-key
extern NSString* const TYDataTypeSaveData;
extern NSString* const TYVersionSaveData;
extern NSString* const TYNumberSaveData;
extern NSString* const TYLocalValuesSaveData;
extern NSString* const TYScriptPointSaveData;
extern NSString* const TYReturnStackSaveData;
// from ver2.00
extern NSString* const TYMakeDateSaveData;
extern NSString* const TYNovelLayerSaveData; 
extern NSString* const TYTextWindowSaveData;
extern NSString* const TYPrintingInfoSaveData;
extern NSString* const TYNovelLayerVisibleSaveData;
extern NSString* const TYBGMPathSaveData;
extern NSString* const TYWavePathSaveData;
extern NSString* const TYSpritesSaveData;
extern NSString* const TYBGImagePathSaveData;
extern NSString* const TYLeftCharSaveData;
extern NSString* const TYCenterCharSaveData;
extern NSString* const TYRightCharSaveData;
extern NSString* const TYMonocroSampleSaveData;
extern NSString* const TYNegaSpecifySaveData;
// from ver2.2
extern NSString* const TYHumanZSaveData;
// from ver2.3
extern NSString* const TYAutoClickTimerSaveData;
extern NSString* const TYClickWaitCursorSaveData;
extern NSString* const TYPageWaitCursorSaveData;
// from ver2.3.1
extern NSString* const TYUnderlineSaveData;
// from ver2.3.2
extern NSString* const TYClickWaitTypeSaveData;
// from ver2.3.3
extern NSString* const TYBGMNoLoopSaveData;
// from ver2.3.4
extern NSString* const TYSpeedModeSaveData;
// from ver2.4
//extern NSString* const TYLoopStackSaveData;
extern NSString* const TYTextgosubIndexSaveData;
extern NSString* const TYEraseTextWindowSavedata;
extern NSString* const TYIspageSaveData;
extern NSString* const TYCustomSelectInfoSaveData;
// from ver2.5
extern NSString* const TYLookbackBufferSaveData;

typedef enum tySaveDataType {
    TYScriptingSaveData,
    TYPrintingWaitSaveData,
} TYSaveDataType;

typedef enum tyClickWaitType {
    TYNormalClickWaitType,
    TYPageClickWaitType,
} TYClickWaitType;

typedef enum tySpeedMode {
    TYUserSpeedMode,
    TYScriptSpeedMode,
} TYSpeedMode;

//#define TYSaveDataVersion 100
//#define TYSaveDataVersion 201
//#define TYSaveDataVersion 233
//#define TYSaveDataVersion 234
#define TYSaveDataVersion 250
#define TYSpriteAlphaFixSaveDataVersion 210
