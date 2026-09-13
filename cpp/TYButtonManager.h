//
//  TYButtonManager.h
//  Tukuyomi
//
//  Created by toveta on Thu Oct 11 2001.
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

#ifndef TYBUTTONMANAGER_H
#define TYBUTTONMANAGER_H

#include <QObject>
#include <QImage>
#include <QVector>
#include <QByteArray>
#include <QNumber>

class TYStageManager;

enum {
    TYButtonUnInitialize = -1,
    TYButtonNoSelected = 0
};

class TYButton : public QObject
{
    Q_OBJECT

public:
    explicit TYButton(QObject* parent = nullptr);
    ~TYButton();

    void initWithID(const QNumber& no, QImage* unselImage, QImage* selImage, const QRect& aRect);
    void drawWithSelect(bool aBool);
    QRect* boundingRect() { return &m_drawRect; }
    QNumber idNo() const { return m_idno; }
    QString description() const;
    bool isOverlay() const { return false; }

private:
    QNumber m_idno;
    QImage* m_unselectedImage;
    QImage* m_selectedImage;
    QRect m_drawRect;
};

class TYButtonManager : public QObject
{
    Q_OBJECT

public:
    explicit TYButtonManager(QObject* parent = nullptr);
    ~TYButtonManager();

    void initWithImage(QImage* srcImage);
    QImage* image() const { return m_sourceImage; }

    void addButton(const QNumber& no, int x, int y, int width, int height, int selx, int sely);

    QByteArray rectMap() const { return m_rectMap; }
    int selectedButtonID();
    int incToIndex();
    int decToIndex();

    void initialButtonImage();
    void drawWithSelection(int status);

    QString description() const;

private:
    TYStageManager* m_stageManager;
    QImage* m_sourceImage;
    QVector<TYButton*> m_btnArray;
    QImage* m_drawBuffer;
    int m_beforeSelection;
    QByteArray m_rectMap;
};

#endif // TYBUTTONMANAGER_H
