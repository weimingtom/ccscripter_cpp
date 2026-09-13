//
//  TYFileLog.m
//  Tukuyomi
//
//  Created by toveta on Sat Nov 10 2001.
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

#include "TYFileLog.h"
#include "TY-NSStringAddition.h"
#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <cstring>
#include <cstdlib>

static void XORMaskToAllBytes(QByteArray& data, char mask)
{
    char* ptr = data.data();
    int len = data.size();
    for (int i = 0; i < len; ++i) {
        ptr[i] ^= mask;
    }
}

TYFileLog::TYFileLog(const QString& path, QObject* parent)
    : QObject(parent)
{
    QFile handle(path);
    
    if (!handle.open(QIODevice::ReadOnly)) {
        return;
    }
    
    QByteArray bufData;
    unsigned char buf;
    
    while (handle.getChar(reinterpret_cast<char*>(&buf))) {
        if (buf == 0x0a)
            break;
        else
            bufData.append(static_cast<char>(buf));
    }
    bufData.append('\0');
    
    int fileNum = atoi(bufData.constData());
    
    for (int i = 0; i < fileNum; i++) {
        int ret = handle.getChar(reinterpret_cast<char*>(&buf));
        if ((!ret) || (buf != '"'))
            break;
            
        bufData.clear();
        
        while (handle.getChar(reinterpret_cast<char*>(&buf)) && (buf != '"')) {
            bufData.append(static_cast<char>(buf));
        }
        
        XORMaskToAllBytes(bufData, 132);
        bufData.append('\0');
        
        QString decoded = QString::fromLocal8Bit(bufData.constData()).toUpper();
        logSet.insert(decoded);
    }
    
    handle.close();
}

TYFileLog::~TYFileLog()
{
}

bool TYFileLog::writeToFile(const QString& path)
{
    QByteArray numbuf;
    numbuf.resize(12);
    int count = logSet.count();
    snprintf(numbuf.data(), 12, "%d\n", count);
    
    QByteArray writeData;
    writeData.append(numbuf);
    
    for (const QString& temp : logSet) {
        writeData.append('"');
        QByteArray data = temp.toLocal8Bit();
        XORMaskToAllBytes(data, 132);
        writeData.append(data);
        writeData.append('"');
    }

    QFile file(path);
    return file.write(writeData) > 0;
}

void TYFileLog::addTYFileLog(TYFileLog* margeLog)
{
    logSet.unite(margeLog->logSet);
}

void TYFileLog::addLog(const QString& filename)
{
    logSet.insert(filename.toUpper());
}

bool TYFileLog::isRead(const QString& filename)
{
    return logSet.contains(filename.toUpper());
}
