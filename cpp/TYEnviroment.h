//
//  TYEnviroment.h
//  Tukuyomi
//
//  Created by toveta on Thu Jan 17 2002.
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


#ifndef TYEnviroment_h
#define TYEnviroment_h

#include <QObject>
#include <QString>
#include <QVariant>

// パッケージ内の定義ファイルを読み込み、さらにアプリのディレクトリ内の環境設定保存ファイルを読み込む。
class TYEnviroment : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString fontName READ fontName WRITE setFontName)
    Q_PROPERTY(QString audioFilePath READ audioFilePath WRITE setAudioFilePath)
    Q_PROPERTY(float bgmVolume READ bgmVolume WRITE setBgmVolume)
    Q_PROPERTY(float voiceVolume READ voiceVolume WRITE setVoiceVolume)
    Q_PROPERTY(float seVolume READ seVolume WRITE setSeVolume)

public:
    static QVariant objectForKey(QString aKey);
    static QVariant defaultObjectForKey(QString aKey);
    static void setObject(QVariant aObj, QString aKey);
    static void save();
    static float floatForKey(QString aKey);
    static bool boolForKey(QString aKey);
    static void reload();

    static QString fontName();
    static void setFontName(const QString& name);
    static QString audioFilePath();
    static void setAudioFilePath(const QString& path);
    static float bgmVolume();
    static void setBgmVolume(float volume);
    static float voiceVolume();
    static void setVoiceVolume(float volume);
    static float seVolume();
    static void setSeVolume(float volume);

private:
    static QVariantMap* packageDict;
    static QVariantMap* userDict;
    static void initialize();

public:
    static const QString ENVDATA_FILENAME;

// constants of dictionry-key
    static const QString TYFontNameEnviroment;
    static const QString TYAudioFilePathEnviroment;
    static const QString TYWindowPointXEnviroment;
    static const QString TYWindowPointYEnviroment;
    static const QString TYBGMVolumeEnviroment;
    static const QString TYVoiceVolumeEnviroment;
    static const QString TYSEVolumeEnviroment;
    static const QString TYScaleInFullScreenModeEnviroment;
    static const QString TYEnableBoldFontEnviroment;
    static const QString TYFullScreenModeEnviroment;
    static const QString TYWaveExtraNameEnviroment;
    static const QString TYDisableSelectScriptEnviroment;
//extern NSString* const TYEnableContextMenuEnviroment;
    static const QString TYLookbackBufferPageEnviroment;
    static const QString TYLookbackVoiceReplayEnviroment;
    static const QString TYDisplayResizeAtFullScreenEnviroment;
    static const QString TYAutomodeWaitTypeEnviroment;
    static const QString TYAutomodeWaitTimeEnviroment;
    static const QString TYKidokumodeEnviroment;
    static const QString TYEnablePlayMovieEnviroment;
    static const QString TYHideOtherFullScreenEnviroment;
    static const QString TYLastSelectedFolderPref;
};

#endif // TYEnviroment_h
