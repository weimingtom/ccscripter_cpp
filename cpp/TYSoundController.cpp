//
//  TYSoundController.m
//  Tukuyomi
//
//  Created by toveta on Sun Oct 07 2001.
//  Copyright (c) 2001 toveta. All rights reserved.
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

#include "TYSoundController.h"
#include "TYResourceServer.h"
//#import "TYWaveBooster.h"
#include "TYWave2Aiff.h"
#include <QDebug>
#include <QFile>
#include <QCoreApplication>

const char* TYSoundStartNotification = "TYSoundStartNotification";
const char* TYSoundFinishNotification = "TYSoundFinishNotification";

TYSoundController::TYSoundController(QObject* parent)
    : QObject(parent)
    , m_playSound(nullptr)
    , m_isLoop(false)
    , m_postingNotification(false)
    , m_isPlaying(false)
    , m_volume(TYSOUND_VOLUME_MAX)
    , m_checkTimer(nullptr)
{
}

TYSoundController::TYSoundController(const QString& path, bool loop, QObject* parent)
    : QObject(parent)
    , m_playSound(nullptr)
    , m_soundPath(path)
    , m_isLoop(loop)
    , m_postingNotification(false)
    , m_isPlaying(false)
    , m_volume(TYSOUND_VOLUME_MAX)
    , m_checkTimer(nullptr)
{
    initWithResource(path, loop, TYSOUND_VOLUME_MAX);
}

TYSoundController::TYSoundController(const QString& path, bool loop, int volume, QObject* parent)
    : QObject(parent)
    , m_playSound(nullptr)
    , m_soundPath(path)
    , m_isLoop(loop)
    , m_postingNotification(false)
    , m_isPlaying(false)
    , m_volume(volume)
    , m_checkTimer(nullptr)
{
    initWithResource(path, loop, volume);
}

void TYSoundController::initWithResource(const QString& path, bool loop, int volume)
{

    if(volume > TYSOUND_VOLUME_MAX)
        volume = TYSOUND_VOLUME_MAX;
    if(volume < 0)
        volume = 0;
    /*
    if(volume==TYSOUND_VOLUME_MAX)
        data = [ [ TYResourceServer sharedServer ] getSoundData:path ];
    else
        data = TYWaveBoost([ [ TYResourceServer sharedServer ] getSoundData:path ],volume);
     */
    m_volume = volume;

    QByteArray data = TYResourceServer::sharedServer()->getSoundData(path);
    //TYVerifyAiff(data);

    // NOTE:Mac OS X 10.3のNSSoundのstopメソッドにはオブジェクトの参照カウントを減らしてしまう致命的なバグがあるため、
    // ここで複数回のretainを行う。
    if (data.isEmpty()) {
        qWarning() << "Failed to load sound data:" << path;
        return;
    }

    // Convert wave to AIFF and apply volume
    QByteArray convData = TYWave2Aiff(data, volume);

    // Save to temp file for QSoundEffect
    QString tempPath = QDir::temp().absoluteFilePath("tysound_XXXXXX.aiff");
    QFile tempFile(tempPath);
    if (tempFile.open(QIODevice::WriteOnly)) {
        tempFile.write(convData);
        tempFile.close();

        m_playSound = new QSoundEffect(this);
        m_playSound->setSource(QUrl::fromLocalFile(tempPath));
        m_playSound->setVolume(volume / 256.0f);

        connect(m_playSound, &QSoundEffect::playingChanged, this, &TYSoundController::onSoundFinished);

        m_soundPath = path;
    }
}


void TYSoundController::play()
{
    if (m_postingNotification && !m_isLoop && m_playSound) {
        QCoreApplication::postEvent(QCoreApplication::instance(),
            new QEvent(QEvent::Type(QEvent::User + 1)));
    }

    if (m_playSound) {
        m_playSound->play();
        m_isPlaying = true;
    } else {
        qWarning() << "Can't play wave file. path=" << m_soundPath;
    }
}

/*
-(void)playIfPlaying
{
    if(isPlaying)
        [ playSound play ];
}
*/

void TYSoundController::stop()
{
    if (m_postingNotification && !m_isLoop) {
        QCoreApplication::postEvent(QCoreApplication::instance(),
            new QEvent(QEvent::Type(QEvent::User + 2)));
    }

    if (m_playSound) {
        m_playSound->stop();
    }
    m_isPlaying = false;
}

void TYSoundController::setPostingNotification(bool aBool)
{
    m_postingNotification = aBool;
}

QString TYSoundController::soundPathIfNeedPlayingAtLoad() const
{
    if (m_playSound) {
        if (m_playSound->isPlaying() && m_isLoop) {
            return m_soundPath;
        }
    }
    return QString();
}

void TYSoundController::onSoundFinished()
{
    if (m_playSound && !m_playSound->isPlaying()) {
        if (m_isLoop) {
            m_playSound->play();
        } else {
            if (m_postingNotification) {
                QCoreApplication::postEvent(QCoreApplication::instance(),
                    new QEvent(QEvent::Type(QEvent::User + 2)));
            }
            emit soundFinished();
        }
    }
}

void TYSoundController::setVolume(int vol)
{
    if (vol > TYSOUND_VOLUME_MAX)
        vol = TYSOUND_VOLUME_MAX;
    if (vol < 0)
        vol = 0;

    m_volume = vol;
    if (m_playSound) {
        m_playSound->setVolume(vol / 256.0f);
    }
}

void TYSoundController::checkPlayingStatus()
{
    if (m_playSound) {
        m_isPlaying = m_playSound->isPlaying();
    }
}


TYSoundController::~TYSoundController()
{
    if (m_playSound) {
        m_playSound->stop();
    }
}

// ----------------------------------------------------------------------------------------
// NSCoding
// ----------------------------------------------------------------------------------------
/*
- (void)encodeWithCoder:(NSCoder *)aCoder
{
    isPlaying=([ playSound isPlaying ] && isLoop ) ? YES : NO;
    
    [ aCoder encodeObject:soundPath ];
    [ aCoder encodeValueOfObjCType:@encode(BOOL) at:&isLoop ];
    [ aCoder encodeValueOfObjCType:@encode(BOOL) at:&isPlaying ];

}

- (id)initWithCoder:(NSCoder *)aDecoder
{
    soundPath = [[aDecoder decodeObject] copy];
    [ aCoder decodeValueOfObjCType:@encode(BOOL) at:&isLoop ];
    [ aCoder decodeValueOfObjCType:@encode(BOOL) at:&isPlaying ];
    
    return self;
}
*/
