//
//  TYAnimationCellImage.h
//  Tukuyomi
//
//  Created by toveta on Wed Feb 27 2002.
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

#ifndef TYANIMATIONCELLIMAGE_H
#define TYANIMATIONCELLIMAGE_H

#include <QObject>
#include <QImage>
#include <QVector>
#include <QTimer>
#include <QString>
#include <QVariant>

class TYCellImage;

// Animation mode enum
enum TYAnimationMode {
    TYAnimationLoopingPlayback = 0,
    TYAnimationNormalPlayback,
    TYAnimationLoopingBackAndForthPlayback,
    TYAnimationNoPlay,
};

// Animation-Infomateion Dictionary-Key
extern const QString TYAnimationModeInfo; // number of constant
extern const QString TYAnimationIntervalInfo; // number or numbers in Array

class TYAnimationCellImage : public QObject {
    Q_OBJECT
public:
    explicit TYAnimationCellImage(const QVector<QImage>& imageArray, const QVariantMap& infoDict, QObject* parent = nullptr);
    ~TYAnimationCellImage();

    void setDelegate(QObject* aObject);
//-(id)delegate;

    void changingCell();

// implemantation for delegate
    void didChangeCell(QObject* sender);

// animation operate
    void play();
    bool isPlaying() const { return m_isPlaying; }
    void stop();
    void resume();

    bool isAnimate() const { return true; }

signals:
    void cellChanged(int index);

private:
    void setNextTimer();

private:
    QObject* m_delegate;
    QVariant m_interval;
    TYAnimationMode m_mode;
    bool m_isPlaying;
    bool m_intervalIsArray;
    bool m_back;

    QVector<QImage> m_imageArray;
    QImage m_currentImage;
    int m_currentCell;

public:
    void changeCell(int index);
    int currentCell() const { return m_currentCell; }
    int cells() const { return m_imageArray.size(); }
    bool isCellImage() const { return true; }
    QImage currentImage() const { return m_currentCell < m_imageArray.size() ? m_imageArray.at(m_currentCell) : QImage(); }
};

#endif // TYANIMATIONCELLIMAGE_H
