//
//  TYScriptStream.h
//  Tukuyomi
//
//  Created by toveta on Wed Oct 17 2001.
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

#ifndef TYScriptStream_h
#define TYScriptStream_h

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QMap>
#include <QList>

// スクリプトファイルからデータを取り出すクラス
class TYScriptStream : public QObject {
    Q_OBJECT

public:
    static TYScriptStream* sharedStream();
    explicit TYScriptStream(QByteArray aData, QObject* parent = nullptr);
    ~TYScriptStream();

    void addNewLine(char* aChar);
    void addLabel(QString label);

    QVariant objectAtIndex(int aIndex);
    char* cStringAtIndex(int aIndex);

    QMap<QString, int> labelDict() const { return m_labelDict; }

private:
    QByteArray m_data;
    QList<QByteArray> m_linePtrArray;
    QMap<QString, int> m_labelDict;
    int m_line;
};

#endif // TYScriptStream_h
