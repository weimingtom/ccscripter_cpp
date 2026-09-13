//
//  TYScriptData.m
//  Tukuyomi
//
//  Created by toveta on Tue Apr 02 2002.
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

#include "TYScriptData.h"
#include <QFile>
#include <QTextStream>
#include <QDir>

TYScriptData::TYScriptData(QObject* parent)
    : QObject(parent)
{
}

TYScriptData::~TYScriptData()
{
}

bool TYScriptData::initWithDirectory(QString path)
{
    QDir dir(path);
    if (!dir.exists()) {
        return false;
    }

    // Load script files
    QStringList filters;
    filters << "*.txt" << "nscript.dat";
    QStringList files = dir.entryList(filters);

    for (const QString& fileName : files) {
        QFile file(dir.filePath(fileName));
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream.setCodec("Shift-JIS");
            while (!stream.atEnd()) {
                m_stringArray.append(stream.readLine());
            }
            file.close();
        }
    }

    return !m_stringArray.isEmpty();
}

bool TYScriptData::initWithTextFiles(QList<QString> pathArray)
{
    for (const QString& path : pathArray) {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream.setCodec("Shift-JIS");
            while (!stream.atEnd()) {
                m_stringArray.append(stream.readLine());
            }
            file.close();
        }
    }
    return true;
}

bool TYScriptData::initWithEncodingFile(QString path)
{
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        stream.setCodec("Shift-JIS");
        while (!stream.atEnd()) {
            m_stringArray.append(stream.readLine());
        }
        file.close();
        return true;
    }
    return false;
}

int TYScriptData::lineOfLabel()
{
    for (int i = 0; i < m_stringArray.size(); ++i) {
        if (m_stringArray[i].startsWith('*')) {
            return i;
        }
    }
    return -1;
}

int TYScriptData::lineOfInstantLabelWithNowLine(int nowLine, bool aBool)
{
    int start = aBool ? m_stringArray.size() - 1 : 0;
    int end = aBool ? -1 : m_stringArray.size();
    int step = aBool ? -1 : 1;

    for (int i = start; i != end; i += step) {
        if (m_stringArray[i].startsWith("*~")) {
            return i;
        }
    }
    return -1;
}

QVariant TYScriptData::stringAtIndex(int aInt)
{
    if (aInt >= 0 && aInt < m_stringArray.size()) {
        return QVariant(m_stringArray[aInt]);
    }
    return QVariant();
}

QVariant TYScriptData::dataAtIndex(int aInt)
{
    if (aInt >= 0 && aInt < m_dataArray.size()) {
        return QVariant(m_dataArray[aInt]);
    }
    return QVariant();
}
