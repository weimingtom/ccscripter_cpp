//
//  TYQTMusicPlayer.m
//  Tukuyomi
//
//  Created by toveta on Thu Aug 29 2002.
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

#include "TYQTMusicPlayer.h"
#include <QVBoxLayout>
#include <QDebug>

TYQTMusicPlayer::TYQTMusicPlayer(QWidget* parent)
    : QWidget(parent)
    , m_player(nullptr)
    , m_volume(1.0f)
    , m_loopMode(NormalPlayback)
    , m_isPlaying(false)
    , m_loopTimer(nullptr)
{
    init();
}

TYQTMusicPlayer::TYQTMusicPlayer(const QRect& geometry, QWidget* parent)
    : QWidget(parent)
    , m_player(nullptr)
    , m_volume(1.0f)
    , m_loopMode(NormalPlayback)
    , m_isPlaying(false)
    , m_loopTimer(nullptr)
{
    setGeometry(geometry);
    init();
}

void TYQTMusicPlayer::init()
{
    m_player = new QMediaPlayer(this);
    connect(m_player, &QMediaPlayer::mediaStatusChanged,
            this, &TYQTMusicPlayer::onMediaStatusChanged);
    connect(m_player, &QMediaPlayer::playbackRateChanged,
            this, &TYQTMusicPlayer::onPlaybackRateChanged);

    m_loopTimer = new QTimer(this);
    connect(m_loopTimer, &QTimer::timeout, this, &TYQTMusicPlayer::start);

    // Set default volume
    m_player->setVolume(static_cast<int>(m_volume * 100));
}

void TYQTMusicPlayer::setMovie(const QString& moviePath)
{
    stop();
    QUrl url = QUrl::fromLocalFile(moviePath);
    setMedia(QMediaContent(url));
}

void TYQTMusicPlayer::setMedia(const QMediaContent& media)
{
    stop();
    m_player->setMedia(media);
}

void TYQTMusicPlayer::start(QObject* sender)
{
    Q_UNUSED(sender);

    if (m_player->media().isNull())
        return;

    if (m_loopMode == LoopingPlayback) {
        // Schedule loop
        qint64 duration = m_player->duration();
        m_loopTimer->setSingleShot(true);
        m_loopTimer->start(duration > 0 ? duration : 1000);
    }

    m_player->play();
    m_isPlaying = true;
}

void TYQTMusicPlayer::stop(QObject* sender)
{
    Q_UNUSED(sender);

    m_loopTimer->stop();
    m_player->stop();
    m_isPlaying = false;
}

bool TYQTMusicPlayer::isPlaying() const
{
    return m_player && (m_player->state() == QMediaPlayer::PlayingState);
}

void TYQTMusicPlayer::gotoBeginning()
{
    if (m_player)
        m_player->setPosition(0);
}

void TYQTMusicPlayer::setVolume(float volume)
{
    m_volume = volume;
    if (m_player)
        m_player->setVolume(static_cast<int>(volume * 100));
}

float TYQTMusicPlayer::volume() const
{
    return m_volume;
}

void TYQTMusicPlayer::setLoopMode(LoopMode mode)
{
    m_loopMode = mode;
}

TYQTMusicPlayer::LoopMode TYQTMusicPlayer::loopMode() const
{
    return m_loopMode;
}

void TYQTMusicPlayer::showController(bool show, bool adjustSize)
{
    Q_UNUSED(show);
    Q_UNUSED(adjustSize);
}

void TYQTMusicPlayer::setEditable(bool editable)
{
    Q_UNUSED(editable);
}

void TYQTMusicPlayer::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
        if (m_loopMode == LoopingPlayback) {
            gotoBeginning();
            start();
        } else {
            m_isPlaying = false;
            emit movieFinished();
        }
    }
}

void TYQTMusicPlayer::onPlaybackRateChanged(qreal rate)
{
    Q_UNUSED(rate);
}
