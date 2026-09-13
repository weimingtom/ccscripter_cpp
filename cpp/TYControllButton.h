//
//  TYControllButton.h
//  Tukuyomi
//
//  Created by toveta on Fri Feb 07 2003.
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
 
#ifndef TYCONTROLLBUTTON_H
#define TYCONTROLLBUTTON_H

#include <QWidget>
#include <QRect>
#include <QString>
#include <QImage>

class TYCellImage;

class TYControllButton : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int tag READ tag WRITE setTag)

public:
    explicit TYControllButton(const QRect& frame, TYCellImage* cellImage, QWidget* parent = nullptr);
    ~TYControllButton();

    TYCellImage* cellImage() const { return m_cellImage; }
    void changeCell(int aIndex);

    void setTarget(QObject* target) { m_target = target; }
    QObject* target() const { return m_target; }

    void setAction(const QString& action) { m_action = action; }
    QString action() const { return m_action; }

    void setTag(int tag) { m_tag = tag; }
    int tag() const { return m_tag; }

    int getTrackingTag() const { return m_trackingTag; }

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    TYCellImage* m_cellImage;
    int m_currentCell;
    QObject* m_target;
    QString m_action;
    int m_tag;
    int m_trackingTag;
};

#endif // TYCONTROLLBUTTON_H
