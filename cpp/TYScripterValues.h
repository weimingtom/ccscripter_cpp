//
//  TYScripterValues.h
//  Tukuyomi
//
//  Created by toveta on Mon Jun 11 2001.
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

#ifndef TYScripterValues_h
#define TYScripterValues_h

#include <QObject>
#include <QString>
#include <QMap>
#include <QList>
#include <QVariant>
#include "TYSaveDataCoding.h"
#include "TYArrayValue.h"

#define NUM_OF_VALUES 10000

// ユーザー定義変数を格納するオブジェクトです。
class TYScripterValues : public QObject, public TYSaveDataCoding {
    Q_OBJECT

public:
    static TYScripterValues* ScripterValues();
    static void setMinGlobalNo(int aInt);

    TYScripterValues();
    ~TYScripterValues();

    QVariant encodeWithSaveData() override;
    void decodeWithSaveData(QVariant aObject) override;

    QString getStringValue(QVariant idno);
    QVariant getIntValue(QVariant idno);
    void setStringValue(QVariant idno, QString setString);
    void setIntValue(QVariant idno, QVariant setNumber);
    void setArrayLine(QVariant idno, QList<QVariant> aArray);
    void setIntLimitID(QVariant idno, QVariant aLow, QVariant aHigh);

    void defineArrayValue(QList<QVariant> aArray);

    bool loadGlobalValues(QString path);
    bool saveGlobalValues(QString path);
    void margeGlobalValues(TYScripterValues* margeValues);
    void resetGame();

private:
    static QVector<int> subarrayFromIndex(const QList<QVariant>& list, int index);

    int values[NUM_OF_VALUES];
    QString strValues[NUM_OF_VALUES];
    QMap<QVariant, TYArrayValue*> arrayValues;
    QMap<QString, QList<QVariant>> limitDict;
};

#endif // TYScripterValues_h
