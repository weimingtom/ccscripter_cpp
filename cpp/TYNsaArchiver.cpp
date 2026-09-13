//
//  TYNsaArchiver.m
//  Tukuyomi
//
//  Created by toveta on Sat Oct 13 2001.
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

#include "TYNsaArchiver.h"
#include "lzss.h"
#include "spb.h"

#include <QFile>
#include <QByteArray>

static TYNsaFileMode NsaArchiverMode = TYNsaFileMode::TYNormalNsaFileMode;

void TYNsaArchiver::setMode(TYNsaFileMode aMode)
{
    NsaArchiverMode = aMode;
}

TYNsaArchiver::TYNsaArchiver(const QString& path, QObject* parent)
    : TYSarArchiver(path, parent)
{
}

bool TYNsaArchiver::initWithContentsOfFile(const QString& path)
{
    QFile handle(path);
    if (!handle.open(QIODevice::ReadOnly)) {
        return false;
    }

    // ファイル数の読み出し
    switch (NsaArchiverMode) {
    case TYNsaFileMode::TY2TypeNsaFileMode:
        handle.read(1);
        break;
    case TYNsaFileMode::TY3TypeNsaFileMode:
        handle.read(2);
        break;
    default:
        break;
    }

    QByteArray readDatabuf = handle.read(2);
    const unsigned char* readbufptr = reinterpret_cast<const unsigned char*>(readDatabuf.constData());
    unsigned filenum = (readbufptr[0] << 8) | readbufptr[1];

    // ファイル数分のオブジェクトへのポインタを格納する領域を確保
    QList<TYNsaArchivedFile*> objectArray;
    QList<QString> keyArray;
    objectArray.reserve(filenum);
    keyArray.reserve(filenum);

    // データ部の始点オフセットの読み出し
    readDatabuf = handle.read(4);
    readbufptr = reinterpret_cast<const unsigned char*>(readDatabuf.constData());
    unsigned localBaseOffset = (readbufptr[0] << 24) | (readbufptr[1] << 16) | (readbufptr[2] << 8) | readbufptr[3];
    switch (NsaArchiverMode) {
    case TYNsaFileMode::TY2TypeNsaFileMode:
        localBaseOffset++;
        break;
    case TYNsaFileMode::TY3TypeNsaFileMode:
        localBaseOffset += 2;
        break;
    default:
        break;
    }

    // 记录路径
    setArcPath(path);
    setBaseOffset(localBaseOffset);

    // 文件的信息表的读取
    for (unsigned i = 0; i < filenum; i++) {
        int j = 0;
        char address[256];

        while (true) {
            QByteArray byteBuf = handle.read(1);
            if (byteBuf.isEmpty()) break;
            unsigned char byte = byteBuf[0];

            if (byte) {
                // バックスラッシュはうざいのでスラッシュに変えます。
                if (byte == '\\') byte = '/';
                address[j] = byte;
            } else {
                address[j] = '\0';
                // アドレスから文字列オブジェクトを作成し、配列に加える。
                // MEMO:ここで日本語の使われたアーカイブで死ぬ可能性がある。NSDataで持つべき。
                keyArray.append(QString::fromLatin1(address));

                // 圧縮タイプの読み出し
                readDatabuf = handle.read(1);
                readbufptr = reinterpret_cast<const unsigned char*>(readDatabuf.constData());
                unsigned fileCompress = *readbufptr;

                // 基点からのオフセットを読み出し
                readDatabuf = handle.read(4);
                readbufptr = reinterpret_cast<const unsigned char*>(readDatabuf.constData());
                unsigned fileOffset = (readbufptr[0] << 24) | (readbufptr[1] << 16) | (readbufptr[2] << 8) | readbufptr[3];

                // ファイルのバイト長を読み出し
                readDatabuf = handle.read(4);
                readbufptr = reinterpret_cast<const unsigned char*>(readDatabuf.constData());
                unsigned fileLength = (readbufptr[0] << 24) | (readbufptr[1] << 16) | (readbufptr[2] << 8) | readbufptr[3];
                
                // ファイルの展開後バイト長を読み出し
                readDatabuf = handle.read(4);
                readbufptr = reinterpret_cast<const unsigned char*>(readDatabuf.constData());
                unsigned fileExLength = (readbufptr[0] << 24) | (readbufptr[1] << 16) | (readbufptr[2] << 8) | readbufptr[3];

                // アーカイブ内ファイルオブジェクトを作成し、配列に加える
                objectArray.append(TYNsaArchivedFile::archivedFileoffset(fileOffset, fileLength, fileCompress, fileExLength));

                break;
            }
            j++;
        }
    }

    // 保存文件索引字典
    for (int i = 0; i < objectArray.size(); i++) {
        setFileAtKey(keyArray[i].toLower(), objectArray[i]);
    }

    return true;
}

// アーカイブ内からファイルを取得してきます。
QByteArray TYNsaArchiver::unArchivedFile(const QString& address)
{
    //NSLog(@"address[%@]\n@",address);
    //NSLog(@"%@\n",[ [ filesDict allKeys ] description ]);
    TYNsaArchivedFile* tmp = qobject_cast<TYNsaArchivedFile*>(fileAtKey(address.toLower()));

    if (!tmp) {
        //NSLog(@"failed search for Archive!!\narichive[%@]\npath[%@]\n",arcPath,address);
        return QByteArray();
    }

    unsigned fileOffset = tmp->fileOffset();
    unsigned fileLength = tmp->fileLength();

    switch (tmp->compressType()) {
    case 0:
            // 圧縮されていないデータならそのまま返す
        return readDataOffset(fileOffset, fileLength);
    case 1:
        // SPB压缩
        return TYGetSpbWithData(readDataOffset(fileOffset, fileLength),
                                fileLength,
                                tmp->expandedLength());
    case 2:
        // LZSS压缩
        return TYDataWithLzssDecodeFromFile(arcPath(), fileOffset + baseOffset(), fileLength, tmp->expandedLength());
    default:
        return QByteArray();
    }

    /*
    if([ tmp compressType ] == 0){
        return [ self readDataOffset:fileOffset length:fileLength ]; // 圧縮されていないデータならそのまま返す
    }
    
    // compressType==2 lzss形式
    if([ tmp compressType ] == 2){
        
    }
     */
    

    
}
