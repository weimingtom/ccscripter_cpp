//
//  TYExtraButtonManager.h
//  Tukuyomi
//
//  Created by toveta on Tue Jun 18 2002.
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

#ifndef TYEXTRABUTTONMANAGER_H
#define TYEXTRABUTTONMANAGER_H

#include <QObject>
#include <QImage>
#include <QVector>
#include <QString>
#include <QNumber>
#include <QByteArray>

class TYStageManager;
class TYCellImage;

enum {
    TYButtonUnInitialize = -1,
    TYButtonNoSelected = 0
};

class TYExtraButton : public QObject
{
    Q_OBJECT

public:
    explicit TYExtraButton(QObject* parent = nullptr);
    ~TYExtraButton();

    void initWithID(const QNumber& no, QImage* selImage, const QRect& aRect);
    void drawWithSelect(bool aBool);
    QRect* boundingRect() { return &m_drawRect; }
    QNumber idNo() const { return m_idno; }
    bool isOverlay() const { return true; }

private:
    QNumber m_idno;
    QImage* m_selectedImage;
    QRect m_drawRect;
};

class TYSpriteButton : public QObject
{
    Q_OBJECT

public:
    explicit TYSpriteButton(QObject* parent = nullptr);
    ~TYSpriteButton();

    void initWithID(const QNumber& no, const QNumber& spriteNo, const QString& action);
    QNumber idNo() const { return m_idno; }
    int spriteNo() const { return m_spriteNo.toInt(); }
    QString appendString() const { return m_appendAction; }

private:
    QNumber m_idno;
    QNumber m_spriteNo;
    QString m_appendAction;
};

class TYCselButton : public QObject
{
    Q_OBJECT

public:
    explicit TYCselButton(QObject* parent = nullptr);
    ~TYCselButton();

    void initWithID(const QNumber& no, int selNo, TYCellImage* image, const QPoint& point);
    void drawWithSelect(bool aBool);
    QNumber idNo() const { return m_idno; }
    int cselNo() const { return m_selNo; }

private:
    QNumber m_idno;
    int m_selNo;
    TYCellImage* m_strImage;
    QPoint m_drawPoint;
};

class TYExtraButtonManager : public QObject
{
    Q_OBJECT

public:
    explicit TYExtraButtonManager(QObject* parent = nullptr);
    ~TYExtraButtonManager();

    void initWithStageManager(TYStageManager* manager, QImage* sourceImage);

    void addButton(const QNumber& no, int x, int y, int width, int height, int selx, int sely);
    void addSpriteButton(const QNumber& no, const QNumber& spriteNo, const QString& action);
    void addCselButton(const QNumber& no, const QNumber& selNo, TYCellImage* image, const QPoint& point);
    void setNoSelectAction(const QString& aStr);

    void initialButtonImage();
    bool changeSelection(int index);
    int selectedButtonID();

    QImage* sourceImage() const { return m_sourceImage; }

private:
    TYStageManager* m_stageManager;
    QImage* m_sourceImage;
    QString m_noSelectAction;
    bool m_isOverlayButton;
    bool m_isSpriteButton;
    bool m_isCustomSel;
    QVector<TYExtraButton*> m_btnArray;
    int m_beforeSelection;
    QImage* m_drawBuffer;
};

#endif // TYEXTRABUTTONMANAGER_H
