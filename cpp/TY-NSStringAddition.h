//
//  TY-NSStringAddition.h
//  Tukuyomi
//
//  Created by toveta on Tue Sep 25 2001.
//  Copyright (c) 2001 toveta. All rights reserved.
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

#ifndef TY_NSStringAddition_h
#define TY_NSStringAddition_h

#include <QString>
#include <QByteArray>
#include <QDataStream>

// Mac OS X日本語環境のデフォルトエンコーディングを使用していると機種依存文字で死ぬので。
// 主にそのへんをなんとかする拡張
class TY_NSStringAddition {
public:
    static QString stringWithCSJISString(const char* cString);
    static QString stringWithCSJISString(const char* cString, unsigned int length);
    
    const char* cSJISString() const;
    QByteArray getCSJISData() const;
    QByteArray sjisData() const;
    unsigned cSJISStringLength() const;
    void getCSJISString(char* buffer) const;
    QString substringSJISRange(int location, int length) const;
};

QString getBackSlashString();

#endif // TY_NSStringAddition_h
