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

#ifndef TYSaveDataCoding_h
#define TYSaveDataCoding_h

#include <QString>
#include <QVariant>

class TYSaveDataCoding
{
public:
    virtual ~TYSaveDataCoding() {}
    virtual QVariant encodeWithSaveData() = 0;
    virtual void decodeWithSaveData(QVariant aObject) = 0;
};

// constants of savedata-key
extern QString const TYDataTypeSaveData;
extern QString const TYVersionSaveData;
extern QString const TYNumberSaveData;
extern QString const TYLocalValuesSaveData;
extern QString const TYScriptPointSaveData;
extern QString const TYReturnStackSaveData;
// from ver2.00
extern QString const TYMakeDateSaveData;
extern QString const TYNovelLayerSaveData;
extern QString const TYTextWindowSaveData;
extern QString const TYPrintingInfoSaveData;
extern QString const TYNovelLayerVisibleSaveData;
extern QString const TYBGMPathSaveData;
extern QString const TYWavePathSaveData;
extern QString const TYSpritesSaveData;
extern QString const TYBGImagePathSaveData;
extern QString const TYLeftCharSaveData;
extern QString const TYCenterCharSaveData;
extern QString const TYRightCharSaveData;
extern QString const TYMonocroSampleSaveData;
extern QString const TYNegaSpecifySaveData;
// from ver2.2
extern QString const TYHumanZSaveData;
// from ver2.3
extern QString const TYAutoClickTimerSaveData;
extern QString const TYClickWaitCursorSaveData;
extern QString const TYPageWaitCursorSaveData;
// from ver2.3.1
extern QString const TYUnderlineSaveData;
// from ver2.3.2
extern QString const TYClickWaitTypeSaveData;
// from ver2.3.3
extern QString const TYBGMNoLoopSaveData;
// from ver2.3.4
extern QString const TYSpeedModeSaveData;
// from ver2.4
//extern NSString* const TYLoopStackSaveData;
extern QString const TYTextgosubIndexSaveData;
extern QString const TYEraseTextWindowSavedata;
extern QString const TYIspageSaveData;
extern QString const TYCustomSelectInfoSaveData;
// from ver2.5
extern QString const TYLookbackBufferSaveData;

enum class TYSaveDataType {
    TYScriptingSaveData,
    TYPrintingWaitSaveData,
};

enum class TYClickWaitType {
    TYNormalClickWaitType,
    TYPageClickWaitType,
};

enum class TYSpeedMode {
    TYUserSpeedMode,
    TYScriptSpeedMode,
};

//#define TYSaveDataVersion 100
//#define TYSaveDataVersion 201
//#define TYSaveDataVersion 233
//#define TYSaveDataVersion 234
#define TYSaveDataVersion 250
#define TYSpriteAlphaFixSaveDataVersion 210

#endif // TYSaveDataCoding_h
