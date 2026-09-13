//
//  TYMiscUtil.m
//  Tukuyomi
//
//  Created by toveta on Wed Sep 26 2001.
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

#include "TYMiscUtil.h"
#include <QString>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QTextStream>
#include <cstdlib>

// NSStringの先頭から数字を取得し、その長さも取得
bool scanIntAndLengthFromString(const QString& source, int* value, int* length)
{
    QString getString;
    bool inNumber = false;
    int startPos = -1;
    
    for (int i = 0; i < source.length(); i++) {
        if (source[i].isDigit()) {
            if (!inNumber) {
                startPos = i;
                inNumber = true;
            }
        } else {
            if (inNumber) {
                getString = source.mid(startPos, i - startPos);
                break;
            }
        }
    }
    
    if (!getString.isEmpty()) {
        *value = getString.toInt();
        *length = getString.length();
        return true;
    }
    
    return false;
}

QString getAppDirectory()
{
    return QCoreApplication::applicationDirPath();
}

static QString StrNScrRootDir;

void setNScrRootDirectory(const QString& aString)
{
    StrNScrRootDir = aString;
}

QString getNScrRootDirectory()
{
    return StrNScrRootDir.isEmpty() ? getAppDirectory() : StrNScrRootDir;
}

// パスのデリミタをMac OS Xのものに変換
QString winPathToUnix(const QString& str)
{
    return str.replace('\\', '/');
}

/*
NSMuatbleDictionary* TYDictionaryWithGlobalSaveData(NSData*)
{
}

NSData* TYGlobalSaveDataWithDictionary(NSDictionary*)
{
}
*/

/*
NSMutableDictionary* TYDictionaryWithFileLog(NSData* aData)
{
}

NSData* TYFileLogWithDictionary(NSDictionary* aDict)
{
}
*/

int TYRandom(int min, max)
{
    return static_cast<int>((max - min + 1) * static_cast<double>(random()) / (static_cast<double>(RAND_MAX) + 1.0)) + min;
}

QString TYWideCharDecimalString(int aInt)
{
    QString aStr = QString::number(aInt);
    return TYWideCharDecimalStringWithString(aStr);
}

QString TYWideCharDecimalStringWithString(const QString& decStr)
{
    QString resultString;
    int len = decStr.length();
    
    for (int i = 0; i < len; i++) {
        QString oneCharStr = decStr.mid(i, 1);
        resultString.append(oneCharStr);
    }
    
    return resultString;
}

QString TYCharacterSet::varNameCharacterSet()
    static QString set;
    
    if (set.isEmpty()) {
        for (int i = 'a'; i <= 'z'; i++) set.append(QChar(i));
        for (int i = 'A'; i <= 'Z'; i++) set.append(QChar(i));
        for (int i = '0'; i <= '9'; i++) set.append(QChar(i));
        set.append('_');
    }
    
    return set;
}

QStringList TYArrayUtil::subarrayFromIndex(const QStringList& list, unsigned int index)
{
    Q_ASSERT(list.count() > static_cast<int>(index));
    
    return list.mid(static_cast<int>(index));
}
