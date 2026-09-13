//
//  TYArrayValue.m
//  Tukuyomi
//
//  Created by toveta on Mon Jan 06 2003.
//  Copyright (c) 2003 toveta. All rights reserved.
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

#include "TYArrayValue.h"


// 配列の指定位置を取り出す再帰関数
int TYArrayValue::get_address(int offset, const QVector<int>& limits, const QVector<int>& address)
{
    if (address.isEmpty()) {
        return 0;
    }

    int tmp1 = limits.first();
    int tmp2 = address.first();

    QVector<int> nextLimits = limits.mid(1);
    QVector<int> nextAddress = address.mid(1);

    if (offset == 0) {
        return tmp2 + get_address(tmp1, nextLimits, nextAddress);
    } else {
        return tmp2 * offset + get_address(offset * tmp1, nextLimits, nextAddress);
    }
}

TYArrayValue::TYArrayValue(int aInt, const QVector<int>& aArray, QObject* parent)
    : QObject(parent)
{
    m_idNo = aInt;
    m_limitArray = aArray;
    // データ格納領域確保
    int size = 1;
    for (int val : aArray) {
        size *= val;
    }

    m_data.resize(size);
    m_lastLimit = aArray.last();
}

void TYArrayValue::setNum(const QVariant& num, const QVector<int>& aArray)
{
    Q_ASSERT(m_limitArray.size() == aArray.size());

    QVector<int> revLimits;
    for (int i = m_limitArray.size() - 1; i >= 0; --i) {
        revLimits.append(m_limitArray[i]);
    }

    QVector<int> revAddress;
    for (int i = aArray.size() - 1; i >= 0; --i) {
        revAddress.append(aArray[i]);
    }

    int address = get_address(0, revLimits, revAddress);
    m_data[address] = num.toInt();
}

void TYArrayValue::setNumbers(const QVector<QVariant>& numbers, const QVector<int>& aArray)
{
    Q_ASSERT(m_limitArray.size() == aArray.size() + 1);

    int prLastLimit = m_limitArray.last();
    Q_ASSERT(m_lastLimit == numbers.size());

    QVector<int> revLimits;
    for (int i = m_limitArray.size() - 1; i >= 0; --i) {
        revLimits.append(m_limitArray[i]);
    }

    QVector<int> revAddress;
    for (int i = aArray.size() - 1; i >= 0; --i) {
        revAddress.append(aArray[i]);
    }

    int address = get_address(revLimits[0], revLimits.mid(1), revAddress);

    // Data assignment
    int* ptr = m_data.data() + address;
    for (const QVariant& val : numbers) {
        *ptr = val.toInt();
        ptr++;
    }
}

QVariant TYArrayValue::getNumOfAddress(const QVector<int>& aArray)
{
    Q_ASSERT(m_limitArray.size() == aArray.size());

    QVector<int> revLimits;
    for (int i = m_limitArray.size() - 1; i >= 0; --i) {
        revLimits.append(m_limitArray[i]);
    }

    QVector<int> revAddress;
    for (int i = aArray.size() - 1; i >= 0; --i) {
        revAddress.append(aArray[i]);
    }

    int address = get_address(0, revLimits, revAddress);

    return QVariant(m_data[address]);
}
