//
//  TYArgmentArray.h
//  Tukuyomi
//
//  Created by toveta on Wed Oct 30 2002.
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


#ifndef TYARGMENTARRAY_H
#define TYARGMENTARRAY_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>


enum class TYParseMode {
    Normal = 0,
    Condition,
    Loop
};

// スクリプト行を解析し、指定されたインデックスのトークンを返すクラス
class TYArgmentArray : public QObject {
    Q_OBJECT

public:
    static TYArgmentArray* arrayWithCString(const char* aCStr, const char* cmdCStr);
    explicit TYArgmentArray(const char* aCStr, const char* cmdCStr, QObject* parent = nullptr);

// パースモードは外から指定する
    void setParseMode(TYParseMode mode);
    void parse();

    QVariant at(int aIndex);
    int count() const;

private:
    QString m_sourceData;
    QStringList m_parsedArgments; // 転送
    bool m_isParsed;
    TYParseMode m_parseMode;
};

#endif // TYARGMENTARRAY_H
