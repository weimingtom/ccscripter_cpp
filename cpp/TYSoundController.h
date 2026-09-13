//
//  TYSoundController.h
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

#ifndef TYSOUNDCONTROLLER_H
#define TYSOUNDCONTROLLER_H

#include <QObject>
#include <QString>
#include <QSoundEffect>
#include <QTimer>

#define TYSOUND_VOLUME_MAX 256


// 効果音を扱うクラス。
class TYSoundController : public QObject {
    Q_OBJECT
private:
    QSoundEffect* m_playSound;
    QString m_soundPath;
    bool m_isLoop;
    bool m_postingNotification;
    bool m_isPlaying;
    int m_volume;
    QTimer* m_checkTimer;
//+(void)setPostingNotification:(BOOL)aBool;
private:
    void initWithResource(const QString& path, bool loop, int volume);
public:
    explicit TYSoundController(QObject* parent = nullptr) : TYEffectGenerater()
    , horizontal(false)
    , reverse(false)
    , phaseInterval(0.0)
    , time(0)
    , numberOfColumn(0)
    , beforePhase(0)
    {
    }
//+(void)setPostingNotification:(BOOL)aBool;

    TYSoundController(const QString& path, bool loop, QObject* parent = nullptr);
    TYSoundController(const QString& path, bool loop, int volume, QObject* parent = nullptr);
//-(void)setSound:(NSString*)soundPath loop:(BOOL)aBool;
    void play();
    void stop();
    void setPostingNotification(bool aBool);
    QString soundPathIfNeedPlayingAtLoad() const;
    int volume() const { return m_volume; }
    void setVolume(int vol);

    bool isPlaying() const { return m_isPlaying; }

signals:
    void soundStarted();
    void soundFinished();

private slots:
    void onSoundFinished();
    void checkPlayingStatus();


public:
    ~TYSoundController();
};

// 効果音設定変更時の通知を受ける。

//
extern const char* TYSoundStartNotification;
extern const char* TYSoundFinishNotification;

#endif // TYSOUNDCONTROLLER_H
