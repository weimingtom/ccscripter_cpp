//
//  TYSaveDataCoding.m
//  Tukuyomi
//
//  Created by toveta on Tue Dec 25 2001.
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

#import "TYSaveDataCoding.h"


NSString* const TYDataTypeSaveData=@"TYDataTypeSaveData";
NSString* const TYVersionSaveData=@"TYVersionSaveData";
NSString* const TYNumberSaveData=@"TYNumberSaveData";
NSString* const TYLocalValuesSaveData=@"TYLocalValuesSaveData";
NSString* const TYScriptPointSaveData=@"TYScriptPointSaveData";
NSString* const TYReturnStackSaveData=@"TYReturnStackSaveData";
NSString* const TYMakeDateSaveData=@"TYMakeDateSaveData";
NSString* const TYTextWindowSaveData=@"TYTextWindowSaveData";
NSString* const TYNovelLayerSaveData=@"TYNovelLayerSaveData";
NSString* const TYPrintingInfoSaveData=@"TYPrintingInfoSaveData";
NSString* const TYNovelLayerVisibleSaveData=@"TYNovelLayerVisibleSaveData";
NSString* const TYSpritesSaveData=@"TYSpritesSaveData";
NSString* const TYBGImagePathSaveData=@"TYBGImagePathSaveData";
NSString* const TYLeftCharSaveData=@"TYLeftCharSaveData";
NSString* const TYCenterCharSaveData=@"TYCenterCharSaveData";
NSString* const TYRightCharSaveData=@"TYRightCharSaveData";
NSString* const TYBGMPathSaveData=@"TYBGMPathSaveData";
NSString* const TYWavePathSaveData=@"TYWavePathSaveData";
NSString* const TYMonocroSampleSaveData=@"TYMonocroSampleSaveData";
NSString* const TYNegaSpecifySaveData=@"TYNegaSpecifySaveData";
NSString* const TYHumanZSaveData=@"TYHumanZSaveData";
NSString* const TYAutoClickTimerSaveData=@"TYAutoClickTimerSaveData";
NSString* const TYClickWaitCursorSaveData=@"TYClickWaitCursorSaveData";
NSString* const TYPageWaitCursorSaveData=@"TYPageWaitCursorSaveData";
NSString* const TYUnderlineSaveData=@"TYUnderlineSaveData";
NSString* const TYClickWaitTypeSaveData=@"TYClickWaitTypeSaveData";
NSString* const TYBGMNoLoopSaveData=@"TYBGMNoLoopSaveData";
NSString* const TYSpeedModeSaveData=@"TYSpeedModeSaveData";
NSString* const TYLoopStackSaveData=@"TYLoopStackSaveData";
NSString* const TYTextgosubIndexSaveData=@"TYTextgosubIndexSaveData";
NSString* const TYEraseTextWindowSavedata=@"TYEraseTextWindowSavedata";
NSString* const TYIspageSaveData=@"TYIspageSaveData";
NSString* const TYCustomSelectInfoSaveData=@"TYCustomSelectInfoSaveData";
NSString* const TYLookbackBufferSaveData=@"TYLookbackBufferSaveData";
