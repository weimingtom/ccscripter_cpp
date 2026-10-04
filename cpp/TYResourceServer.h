//
//  TYResourceServer.h
//  Tukuyomi
//
//  Created by toveta on Thu Jun 14 2001.
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

#ifndef TYResourceServer_h
#define TYResourceServer_h

#include <QObject>
#include <QString>
#include <QImage>

class TYEffectPatternMap;

// NScripterのシナリオファイル以外の外部リソースを管理します。
// リソースの要求があった場合、まず各アーカイバの仮想パスとして検索し、次にディスク内の実ファイルを探します。
class TYResourceServer : public QObject {
    Q_OBJECT
public:
    static TYResourceServer* sharedServer();

private:
    explicit TYResourceServer(QObject* parent = nullptr);
    ~TYResourceServer();

public:
    void addArchiver(const QString& path);

    const QString getFilePath(const QString& path); // ディスク中にファイルが存在するならそのパスを返す。
//    NSData* getData(const QString& path); // プラグインを使用せずにデータ取得。
//    NSImage* getImage(const QString& path, bool transMode, bool animate);
//    NSImage* getImage(const QString& path, bool transMode); // イメージを取得し、タグに従い加工して返す
//    NSImage* getImageFromString(const QString& str);
//    NSBitmapImageRep* getBitmap(const QString& path);
//    NSImage* transrateImageFromBitmap(NSBitmapImageRep* bitmap, const QString& transMode); // private
//    NSData* getSoundData(const QString& path);
    TYEffectPatternMap* getEffectPattern(const QString& path);
    QString getFilePathMakeTemp(const QString& path); // ファイルがディスク中に存在しなければ、Tempファイルを作成し、そのパスを返す。

    void addSpi(const QString& pluginName, const QString& extension);
    void addSoundPressPlugin(const QString& pluginName, const QString& extension);
    void setDefaultTransMode(const QString& transmode);
    void addEffectPattern(const QString& path);

    void executeBundle(const QString& pathAndArg);


    bool filelog(const QString& path);
    bool addlog(const QString& filename);
    bool fchk(const QString& filename);
    bool saveLog(const QString& path);
};

#define FILELOG_FILENAME "NScrflog.dat"
#define WIN_PATH_DELIMITER "\\"

#endif // TYResourceServer_h

