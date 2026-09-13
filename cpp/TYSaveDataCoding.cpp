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

#include "TYSaveDataCoding.h"

QString const TYDataTypeSaveData = "TYDataTypeSaveData";
QString const TYVersionSaveData = "TYVersionSaveData";
QString const TYNumberSaveData = "TYNumberSaveData";
QString const TYLocalValuesSaveData = "TYLocalValuesSaveData";
QString const TYScriptPointSaveData = "TYScriptPointSaveData";
QString const TYReturnStackSaveData = "TYReturnStackSaveData";
QString const TYMakeDateSaveData = "TYMakeDateSaveData";
QString const TYTextWindowSaveData = "TYTextWindowSaveData";
QString const TYNovelLayerSaveData = "TYNovelLayerSaveData";
QString const TYPrintingInfoSaveData = "TYPrintingInfoSaveData";
QString const TYNovelLayerVisibleSaveData = "TYNovelLayerVisibleSaveData";
QString const TYSpritesSaveData = "TYSpritesSaveData";
QString const TYBGImagePathSaveData = "TYBGImagePathSaveData";
QString const TYLeftCharSaveData = "TYLeftCharSaveData";
QString const TYCenterCharSaveData = "TYCenterCharSaveData";
QString const TYRightCharSaveData = "TYRightCharSaveData";
QString const TYBGMPathSaveData = "TYBGMPathSaveData";
QString const TYWavePathSaveData = "TYWavePathSaveData";
QString const TYMonocroSampleSaveData = "TYMonocroSampleSaveData";
QString const TYNegaSpecifySaveData = "TYNegaSpecifySaveData";
QString const TYHumanZSaveData = "TYHumanZSaveData";
QString const TYAutoClickTimerSaveData = "TYAutoClickTimerSaveData";
QString const TYClickWaitCursorSaveData = "TYClickWaitCursorSaveData";
QString const TYPageWaitCursorSaveData = "TYPageWaitCursorSaveData";
QString const TYUnderlineSaveData = "TYUnderlineSaveData";
QString const TYClickWaitTypeSaveData = "TYClickWaitTypeSaveData";
QString const TYBGMNoLoopSaveData = "TYBGMNoLoopSaveData";
QString const TYSpeedModeSaveData = "TYSpeedModeSaveData";
QString const TYLoopStackSaveData = "TYLoopStackSaveData";
QString const TYTextgosubIndexSaveData = "TYTextgosubIndexSaveData";
QString const TYEraseTextWindowSavedata = "TYEraseTextWindowSavedata";
QString const TYIspageSaveData = "TYIspageSaveData";
QString const TYCustomSelectInfoSaveData = "TYCustomSelectInfoSaveData";
QString const TYLookbackBufferSaveData = "TYLookbackBufferSaveData";
