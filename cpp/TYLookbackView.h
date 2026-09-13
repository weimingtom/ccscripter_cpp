//
//  TYLookbackView.h
//  Tukuyomi
//
//  Created by toveta on Thu Mar 06 2003.
//  Copyright (c) 2003 toveta All rights reserved.
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

#ifndef TYLOOKBACKVIEW_H
#define TYLOOKBACKVIEW_H

#include <QWidget>
#include <QImage>
#include <QVector>
#include <QRect>
#include <QString>
#include <QMap>
#include <QVariant>

class TYLookbackLayer;
class TYControllButton;
class TYCellImage;
class TYController;

class TYLookbackView : public QWidget
{
    Q_OBJECT

public:
    explicit TYLookbackView(QWidget* parent = nullptr);
    ~TYLookbackView();

    bool initWithFrame(const QRect& aFrame,
                       QObject* aObj,
                       const QMap<QString, QVariant>& aDict,
                       const QMap<QString, QVariant>& layoutDict,
                       const QVector<QImage*>& buffer,
                       TYCellImage* uImage,
                       TYCellImage* dImage);

    void setPageIndex(int aInt);
    void updatesTrackingRect();

    void endMode();

    void up_action();
    void down_action();
    void cancel_action(QObject* sender);
    void execButton(QObject* sender);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    QVector<QImage*> m_lookbackArray;
    int m_index;
    //NSColor *menuWindowColor;	
    QMap<QString, QVariant> m_attDict;
    QObject* m_controller;
    QImage* m_drawBuffer;
    TYControllButton* m_button[2];
    int m_rectTag[2];
    QRect m_btnRect[2];
};

#endif // TYLOOKBACKVIEW_H
