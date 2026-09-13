//
//  TYInputStrController.h
//  Tukuyomi
//
//  Created by toveta on Thu Jan 24 2002.
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

#ifndef TYInputStrController_h
#define TYInputStrController_h

#include <QObject>
#include <QString>

class QDialog;
class QLineEdit;
class QLabel;
class QPushButton;

class TYInputStrController : public QObject
{
    Q_OBJECT

public:
    static TYInputStrController* dialog();

    explicit TYInputStrController(QObject* parent = nullptr);
    ~TYInputStrController();

    int runModalCaption(const QString& aStr, int aInt, bool aBool);
    int runModalCaption(const QString& aStr, const QString& defStr, int aInt, bool aBool);
    QString string() const;

public slots:
    void dialogOk();

signals:
    void textChanged(const QString& text);

protected slots:
    void textDidChange(const QString& newStr);

private:
    QDialog* m_window;
    QLabel* m_caption;
    QLineEdit* m_input;
    QPushButton* m_okButton;

    QString m_beforeString;
    int m_length;
    bool m_notAscii; // 全角のみ受け付けるフラグ
};

// notification methods of NSTextField
#endif // TYInputStrController_h
