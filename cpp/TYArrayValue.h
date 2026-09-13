//
//  TYArrayValue.h
//  Tukuyomi
//
//  Created by toveta on Mon Jan 06 2003.
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

#ifndef TYARRAYVALUE_H
#define TYARRAYVALUE_H

#include <QObject>
#include <QVariant>
#include <QVector>


// 配列変数クラス
class TYArrayValue : public QObject
{
    Q_OBJECT

public:
    TYArrayValue(int aInt, const QVector<int>& aArray, QObject* parent = nullptr);

    // Set single value
    void setNum(const QVariant& num, const QVector<int>& aArray);

    // Set multiple values for a row
    void setNumbers(const QVector<QVariant>& numbers, const QVector<int>& aArray);

    // Get value
    QVariant getNumOfAddress(const QVector<int>& aArray);

    // NSCoding support
    void encodeWithCoder(void* coder);
    static TYArrayValue* initWithCoder(void* decoder);

private:
    static int get_address(int offset, const QVector<int>& limits, const QVector<int>& address);

    int m_idNo;
    QVector<int> m_limitArray;
    QVector<int> m_data;
    int m_lastLimit;
};

// 単体でセット
// 行に対して連続でセット
// 値を取得
#endif // TYARRAYVALUE_H
