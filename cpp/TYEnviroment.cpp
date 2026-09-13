//
//  TYEnviroment.m
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

#include "TYEnviroment.h"
#include "TYMiscUtil.h"
#include <QCoreApplication>
#include <QSettings>
#include <QDir>

QVariantMap* TYEnviroment::packageDict = nullptr;
QVariantMap* TYEnviroment::userDict = nullptr;

const QString TYEnviroment::ENVDATA_FILENAME = "envdata.plist";

const QString TYEnviroment::TYFontNameEnviroment = "DefaultFontName";
const QString TYEnviroment::TYAudioFilePathEnviroment = "DefaultAudioTrackPath";
const QString TYEnviroment::TYWindowPointXEnviroment = "WindowPointX";
const QString TYEnviroment::TYWindowPointYEnviroment = "WindowPointY";
const QString TYEnviroment::TYBGMVolumeEnviroment = "BGMVolume";
const QString TYEnviroment::TYVoiceVolumeEnviroment = "VoiceVolume";
const QString TYEnviroment::TYSEVolumeEnviroment = "SEVolume";
const QString TYEnviroment::TYScaleInFullScreenModeEnviroment = "ScaleInFullScreenMode";
const QString TYEnviroment::TYEnableBoldFontEnviroment = "EnableBoldFont";
const QString TYEnviroment::TYFullScreenModeEnviroment = "isFullScreen";
const QString TYEnviroment::TYWaveExtraNameEnviroment = "WAVExtraBundleName";
const QString TYEnviroment::TYDisableSelectScriptEnviroment = "DisableSelectScript";
//NSString* const TYEnableContextMenuEnviroment=@"EnableContextMenu";
const QString TYEnviroment::TYLookbackBufferPageEnviroment = "LookbackBufferPage";
const QString TYEnviroment::TYLookbackVoiceReplayEnviroment = "LookbackVoiceReplay";
const QString TYEnviroment::TYDisplayResizeAtFullScreenEnviroment = "DisplayResizeInFullScreenMode";
const QString TYEnviroment::TYAutomodeWaitTypeEnviroment = "AutomodeWaitType";
const QString TYEnviroment::TYAutomodeWaitTimeEnviroment = "AutomodeWaitTime";
const QString TYEnviroment::TYKidokumodeEnviroment = "Kidokumode";
const QString TYEnviroment::TYEnablePlayMovieEnviroment = "EnablePlayMovie";
const QString TYEnviroment::TYHideOtherFullScreenEnviroment = "HideOtherInFullScreenMode";
const QString TYEnviroment::TYLastSelectedFolderPref = "LastSelectedFolder";

void TYEnviroment::initialize()
{
    // Load package settings from application bundle
    QString settingsPath = QCoreApplication::applicationDirPath() + "/Settings.plist";
    if (QFile::exists(settingsPath)) {
        QSettings settings(settingsPath, QSettings::NativeFormat);
        packageDict = new QVariantMap();
        for (const QString& key : settings.allKeys()) {
            (*packageDict)[key] = settings.value(key);
        }
    }

    // Load user settings
    QString userPath = getNScrRootDirectory() + "/" + ENVDATA_FILENAME;
    if (QFile::exists(userPath)) {
        QSettings userSettings(userPath, QSettings::NativeFormat);
        userDict = new QVariantMap();
        for (const QString& key : userSettings.allKeys()) {
            (*userDict)[key] = userSettings.value(key);
        }
    } else {
        userDict = new QVariantMap();
    }
}

QVariant TYEnviroment::objectForKey(QString aKey)
{
    if (!packageDict || !userDict) {
        initialize();
    }

    if (userDict->contains(aKey)) {
        return (*userDict)[aKey];
    } else if (packageDict && packageDict->contains(aKey)) {
        return (*packageDict)[aKey];
    }

    return QVariant();
}

float TYEnviroment::floatForKey(QString aKey)
{
    return objectForKey(aKey).toFloat();
}

bool TYEnviroment::boolForKey(QString aKey)
{
    return objectForKey(aKey).toBool();
}

QVariant TYEnviroment::defaultObjectForKey(QString aKey)
{
    if (!packageDict) {
        initialize();
    }
    if (packageDict && packageDict->contains(aKey)) {
        return (*packageDict)[aKey];
    }
    return QVariant();
}

void TYEnviroment::setObject(QVariant aObj, QString aKey)
{
    if (!userDict) {
        userDict = new QVariantMap();
    }
    (*userDict)[aKey] = aObj;
}

void TYEnviroment::save()
{
    if (!userDict) {
        return;
    }

    QString path = getNScrRootDirectory() + "/" + ENVDATA_FILENAME;
    QSettings settings(path, QSettings::NativeFormat);
    for (auto it = userDict->constBegin(); it != userDict->constEnd(); ++it) {
        settings.setValue(it.key(), it.value());
    }
}

void TYEnviroment::reload()
{
    if (userDict) {
        delete userDict;
        userDict = nullptr;
    }
    initialize();
}

QString TYEnviroment::fontName()
{
    return objectForKey(TYFontNameEnviroment).toString();
}

void TYEnviroment::setFontName(const QString& name)
{
    setObject(name, TYFontNameEnviroment);
}

QString TYEnviroment::audioFilePath()
{
    return objectForKey(TYAudioFilePathEnviroment).toString();
}

void TYEnviroment::setAudioFilePath(const QString& path)
{
    setObject(path, TYAudioFilePathEnviroment);
}

float TYEnviroment::bgmVolume()
{
    return objectForKey(TYBGVolumeEnviroment).toFloat();
}

void TYEnviroment::setBgmVolume(float volume)
{
    setObject(volume, TYBGVolumeEnviroment);
}

float TYEnviroment::voiceVolume()
{
    return objectForKey(TYVoiceVolumeEnviroment).toFloat();
}

void TYEnviroment::setVoiceVolume(float volume)
{
    setObject(volume, TYVoiceVolumeEnviroment);
}

float TYEnviroment::seVolume()
{
    return objectForKey(TYSEVolumeEnviroment).toFloat();
}

void TYEnviroment::setSeVolume(float volume)
{
    setObject(volume, TYSEVolumeEnviroment);
}
