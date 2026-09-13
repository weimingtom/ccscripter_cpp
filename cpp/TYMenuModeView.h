//
//  TYMenuModeView.h
//  Tukuyomi
//
//  Created by toveta on Thu Feb 06 2003.
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

#ifndef TYMENUMODEVIEW_H
#define TYMENUMODEVIEW_H

#include <QWidget>
#include <QColor>
#include <QMap>
#include <QVector>
#include <QString>

class TYController;
class TYControllButton;

class TYMenuModeView : public QWidget
{
    Q_OBJECT

public:
    explicit TYMenuModeView(QWidget* parent = nullptr);
    ~TYMenuModeView();

    void setController(QObject* aObj,
                       const QVector<TYControllButton*>& bArray,
                       const QVector<QString>& aArray,
                       const QColor& aColor);

    void updatesTrackingRect();
    void removeAllTrackingRect();
    void setButtonArray(const QVector<TYControllButton*>& aArray);
    void setActionArray(const QVector<QString>& aArray);
    void execButton(QObject* sender = nullptr);

    void endMode();

    void cancel_action(QObject* sender = nullptr);
    void select_action();
    void up_action();
    void down_action();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    bool event(QEvent* event) override;

private:
    void mouseEntered(TYControllButton* btn);
    void mouseExited(TYControllButton* btn);

    TYController* m_controller;
    QColor m_menuWindowColor;
    QMap<TYControllButton*, bool> m_trackingButtons;
    QVector<TYControllButton*> m_btnArray;
    QVector<QString> m_actArray;
    TYControllButton* m_selectedButton;
};

#endif // TYMENUMODEVIEW_H
