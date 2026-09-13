//
//  TYScripterValues.m
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


#include "TYScripterValues.h"
#include "TYMiscUtil.h"
#include <QFile>
#include <QDataStream>

static int MIN_GLOBAL_IDNO = 200; // これ以上のidnoならグローバル変数
#define MAX_GLOBAL_IDNO 4095 // ここまでのidnoならグローバル変数

static TYScripterValues* sharedScripterValues = nullptr;

TYScripterValues::TYScripterValues()
{
    for (int i = 0; i < NUM_OF_VALUES; i++) {
        strValues[i] = QStringLiteral("");
    }
}

TYScripterValues::~TYScripterValues()
{
    // QString handles memory automatically
    qDeleteAll(arrayValues);
}

TYScripterValues* TYScripterValues::ScripterValues()
{
    if (!sharedScripterValues) {
        sharedScripterValues = new TYScripterValues();
    }
    return sharedScripterValues;
}

void TYScripterValues::setMinGlobalNo(int aInt)
{
    MIN_GLOBAL_IDNO = aInt;
}

QVector<int> TYScripterValues::subarrayFromIndex(const QList<QVariant>& list, int index)
{
    QVector<int> result;
    for (int i = index; i < list.size(); ++i) {
        result.append(list[i].toInt());
    }
    return result;
}

QVariant TYScripterValues::encodeWithSaveData()
{
    // ローカル変数の値をNSDataに詰めて返す。形式はGlobalと同じ。
    QByteArray saveData;
    saveData.reserve(MIN_GLOBAL_IDNO * 5);
    unsigned char buf[4];
    for (int i = 0; i < MIN_GLOBAL_IDNO; i++){
        // 数字変数の格納。リトルエンディアンのint型で格納する。
        buf[0] = values[i] & 0xFF;
        buf[1] = ( values[i] >> 8 ) & 0xFF;
        buf[2] = ( values[i] >> 16 ) & 0xFF;
        buf[3] = ( values[i] >> 24 ) & 0xFF;

        saveData.append(reinterpret_cast<const char*>(buf), 4);

        // 文字列変数の格納。C文字列に変換して格納する。
        QByteArray strData = strValues[i].toLocal8Bit();
        saveData.append(strData);
        saveData.append('\0');
    }

    // Encode arrayValues as a QVariantMap
    QVariantMap arrayValuesMap;
    for (auto it = arrayValues.constBegin(); it != arrayValues.constEnd(); ++it) {
        // Store TYArrayValue data as a QVariant
        TYArrayValue* arrVal = it.value();
        QVariantMap arrValMap;
        arrValMap["idNo"] = arrVal->property("idNo").toInt();
        // Store the raw data bytes
        // Note: Full TYArrayValue serialization would require encodeWithCoder support
        arrayValuesMap[it.key().toString()] = QVariant::fromValue(arrValMap);
    }

    QVariantList result;
    result.append(saveData);
    result.append(arrayValuesMap);
    return result;
}

void TYScripterValues::decodeWithSaveData(QVariant aObject)
{
    // encodeされたデータからローカル変数を復元する。
    if (aObject.canConvert<QVariantList>()) {
        QVariantList list = aObject.toList();
        if (list.size() >= 2) {
            QByteArray data = list[0].toByteArray();
            QVariantMap arrMap = list[1].toMap();

            // Clear and rebuild arrayValues
            qDeleteAll(arrayValues);
            arrayValues.clear();

            const unsigned char* ptr = reinterpret_cast<const unsigned char*>(data.constData());

            for (int i = 0; i < MIN_GLOBAL_IDNO; i++) {
                values[i] = ptr[0] + (ptr[1] << 8) + (ptr[2] << 16) + (ptr[3] << 24);
                ptr += 4;

                int len = qstrlen(reinterpret_cast<const char*>(ptr));
                strValues[i] = QString::fromLocal8Bit(reinterpret_cast<const char*>(ptr));
                ptr += len + 1;
            }

            // Note: Full TYArrayValue reconstruction from encoded data requires
            // implementing initWithCoder on TYArrayValue
            Q_UNUSED(arrMap);
        }
    } else if (aObject.canConvert<QByteArray>()) {
        QByteArray data = aObject.toByteArray();
        const unsigned char* ptr = reinterpret_cast<const unsigned char*>(data.constData());

        for (int i = 0; i < MIN_GLOBAL_IDNO; i++) {
            values[i] = ptr[0] + (ptr[1] << 8) + (ptr[2] << 16) + (ptr[3] << 24);
            ptr += 4;

            int len = qstrlen(reinterpret_cast<const char*>(ptr));
            strValues[i] = QString::fromLocal8Bit(reinterpret_cast<const char*>(ptr));
            ptr += len + 1;
        }
    }
}

QString TYScripterValues::getStringValue(QVariant idno)
{
    return strValues[idno.toInt()];
}

QVariant TYScripterValues::getIntValue(QVariant idno)
{
    if (idno.canConvert<int>()) {
        int intId = idno.toInt();
        Q_ASSERT_X(intId >= 0 && intId < NUM_OF_VALUES,
                   "TYScripterValues", "int value reference overflow");
        return QVariant(values[intId]);
    } else {
        // 配列変数の場合
        QList<QVariant> list = idno.toList();
        if (list.isEmpty()) {
            return QVariant();
        }
        TYArrayValue* array = arrayValues.value(list.first(), nullptr);
        if (array) {
            return array->getNumOfAddress(subarrayFromIndex(list, 1));
        }
        return QVariant();
    }
}

void TYScripterValues::setStringValue(QVariant idno, QString setString)
{
    int intId = idno.toInt();
    Q_ASSERT_X(intId >= 0 && intId < NUM_OF_VALUES,
               "TYScripterValues", "string value reference overflow");
    strValues[intId] = setString;
}

void TYScripterValues::setIntValue(QVariant idno, QVariant setNumber)
{
    if (idno.canConvert<int>()) {
        int intId = idno.toInt();
        Q_ASSERT_X(intId >= 0 && intId < NUM_OF_VALUES,
                   "TYScripterValues", "int value reference overflow");

        values[intId] = setNumber.toInt();

        QString key = idno.toString();
        if (limitDict.contains(key)) {
            QList<QVariant> limitArray = limitDict.value(key);
            if (values[intId] < limitArray.value(0).toInt()) {
                values[intId] = limitArray.value(0).toInt();
            } else if (values[intId] > limitArray.value(1).toInt()) {
                values[intId] = limitArray.value(1).toInt();
            }
        }
    } else {
        // 攝楍曄悢偺応崌
        QList<QVariant> list = idno.toList();
        if (list.isEmpty()) {
            return;
        }
        TYArrayValue* value = arrayValues.value(list.first(), nullptr);
        if (value) {
            value->setNum(setNumber, subarrayFromIndex(list, 1));
        }
    }
}

void TYScripterValues::setArrayLine(QVariant idno, QList<QVariant> aArray)
{
    QList<QVariant> list = idno.toList();
    if (list.isEmpty()) {
        return;
    }
    TYArrayValue* value = arrayValues.value(list.first(), nullptr);
    if (value) {
        QVector<QVariant> numbers;
        for (const QVariant& v : aArray) {
            numbers.append(v);
        }
        value->setNumbers(numbers, subarrayFromIndex(list, 1));
    }
}

void TYScripterValues::defineArrayValue(QList<QVariant> aArray)
{
    if (aArray.isEmpty()) {
        return;
    }
    QVariant idNo = aArray.first();

    // 揧帤+1偺挿偝傪幚嵺偵偼妋曐偡傞昁梫偑偁傞偨傔丄偙偙偱壛岺偡傞
    QVector<int> incArray;
    for (int i = 1; i < aArray.size(); ++i) {
        incArray.append(aArray[i].toInt() + 1);
    }

    TYArrayValue* arrValue = new TYArrayValue(idNo.toInt(), incArray);
    arrayValues.insert(idNo, arrValue);
}

void TYScripterValues::setIntLimitID(QVariant idno, QVariant aLow, QVariant aHigh)
{
    QList<QVariant> limitArray;
    limitArray.append(aLow);
    limitArray.append(aHigh);
    limitDict[idno.toString()] = limitArray;
}

bool TYScripterValues::loadGlobalValues(QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    for (int i = MIN_GLOBAL_IDNO; i < MAX_GLOBAL_IDNO; i++) {
        unsigned char buf[4];
        if (file.read(reinterpret_cast<char*>(buf), 4) != 4) {
            break;
        }

        values[i] = buf[0] + (buf[1] << 8) + (buf[2] << 16) + (buf[3] << 24);

        QByteArray strData;
        char byte;
        while (file.getChar(&byte)) {
            if (byte == '\0') {
                break;
            }
            strData.append(byte);
        }
        if (!strData.isEmpty()) {
            strValues[i] = QString::fromLocal8Bit(strData);
        }
    }

    file.close();
    return true;
}

bool TYScripterValues::saveGlobalValues(QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    for (int i = MIN_GLOBAL_IDNO; i < MAX_GLOBAL_IDNO; i++) {
        unsigned char buf[4];
        buf[0] = values[i] & 0xFF;
        buf[1] = (values[i] >> 8) & 0xFF;
        buf[2] = (values[i] >> 16) & 0xFF;
        buf[3] = (values[i] >> 24) & 0xFF;

        file.write(reinterpret_cast<const char*>(buf), 4);

        QByteArray strData = strValues[i].toLocal8Bit();
        file.write(strData);
        file.putChar('\0');
    }

    file.close();
    return true;
}

void TYScripterValues::margeGlobalValues(TYScripterValues* margeValues)
{
    for (int i = MIN_GLOBAL_IDNO; i < MAX_GLOBAL_IDNO; i++) {
        if (!values[i]) {
            values[i] = margeValues->values[i];
        }
        if (strValues[i].isEmpty()) {
            strValues[i] = margeValues->strValues[i];
        }
    }
}

void TYScripterValues::resetGame()
{
    for (int i = 0; i < MIN_GLOBAL_IDNO; i++) {
        values[i] = 0;
        strValues[i] = QStringLiteral("");
    }
}
