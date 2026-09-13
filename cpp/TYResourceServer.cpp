//
//  TYResourceServer.m
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

#include "TYResourceServer.h"
#include "TYImageUtil.h"
#include "TY-NSStringAddition.h"
#include "TY_NSDataAddtion.h"
#include "TYNBZDecompresser.h"
#include "TYAnimationCellImage.h"
#include "TYStageManager.h"
#include "TYStringImage.h"
#include "TYMiscUtil.h"
#include "TYEffectPatternMap.h"
#include "TYCCSProxy.h"
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QByteArray>
#include <QUuid>

static TYResourceServer* sharedServer = nullptr;

TYResourceServer::TYResourceServer(QObject* parent)
    : QObject(parent)
    , ty_archivers()
    , effectPatternDict()
    , spiPluginDict()
    , soundPressPluginDict()
    , filelogDict()
    , executableBundles()
    , defaultTransmode(TYTRANSMODE_COPY)
    , filelog(false)
{
    // 添加NBZ压缩插件支持
    addSoundPressPlugin(QString(), QStringLiteral("nbz"));
}

TYResourceServer::~TYResourceServer()
{
}

TYResourceServer* TYResourceServer::sharedServer()
{
    if (!sharedServer) {
        sharedServer = new TYResourceServer();
    }
    return sharedServer;
}

void TYResourceServer::addArchiver(const QString& path)
{
    TYArchiver* arc;
    QString arcPath;
    
    // path中包含DLL规范时进行分割
    QStringList parts = path.split(QLatin1String("|"));
    arcPath = parts.at(0);
    
    QString fullPath = getNScrRootDirectory() + QDir::separator() + arcPath;
    arc = new TYArchiver(fullPath);
    
    if (arc) {
        ty_archivers.append(arc);
    }
}

QString TYResourceServer::getFilePath(const QString& path)
{
    QString convPath = winPathToUnix(path);
    convPath = getNScrRootDirectory() + QDir::separator() + convPath;
    
    QFileInfo fileInfo(convPath);
    return fileInfo.exists() ? convPath : QString();
}

QString TYResourceServer::getFilePathMakeTemp(const QString& path)
{
    QString diskPath = getFilePath(path);
    if (!diskPath.isEmpty())
        return diskPath;
    else
        return QString();
}

QByteArray TYResourceServer::getData(const QString& path)
{
    if (path.isEmpty()) {
        return QByteArray();
    }
    
    QString convPath = winPathToUnix(path);
    
    // 从归档中查找
    for (TYArchiver* arc : ty_archivers) {
        QByteArray data = arc->unArchivedFile(convPath);
        if (!data.isEmpty())
            return data;
    }
    
    // 从磁盘文件读取
    convPath = getNScrRootDirectory() + QDir::separator() + convPath;
    QFile file(convPath);
    if (file.open(QIODevice::ReadOnly)) {
        return file.readAll();
    }
    
    return QByteArray();
}

QImage TYResourceServer::getImage(const QString& path, bool transMode, bool isAnimate)
{
    QString filePath;
    QStringList pathArray;
    QStringList tagArray;
    QString transmode;
    QImage bitmap;
    int intervalValue = 0;
    int loopType = 0;
    
    // 解析路径标签
    pathArray = path.split(QLatin1Char(';'));
    if (pathArray.count() != 1) {
        tagArray = pathArray.at(0).split(QLatin1Char('/'));
    }
    
    if (!transMode) {
        transmode = TYTRANSMODE_COPY;
    } else if (path.length() > 2 && path.startsWith(QLatin1String(":s"))) {
        // 字符串图像
        return getImageFromString(path.mid(2));
    } else if (pathArray.count() == 1) {
        transmode = defaultTransmode;
    } else {
        transmode = tagArray.at(0);
        transmode = transmode.mid(1);
    }
    
    if (filelog) {
        addlog(path);
    }
    
    // 获取文件路径
    filePath = winPathToUnix(pathArray.last());
    
    // 获取位图
    QVariant decodeResult = getBitmap(filePath);
    if (decodeResult.canConvert<QImage>()) {
        bitmap = decodeResult.value<QImage>();
    } else if (decodeResult.canConvert<QByteArray>()) {
        // 从字节数组创建图像
        QByteArray data = decodeResult.value<QByteArray>();
        bitmap = QImage::fromData(data);
    }
    
    if (bitmap.isNull()) {
        qWarning() << "failed to load Image from Archives and disk" << path;
        return QImage();
    }
    
    // 动画图像处理
    if (isAnimate && tagArray.count() == 2) {
        QString animeOperate = tagArray.last();
        QStringList animeOperateArray = animeOperate.split(QLatin1Char(','));
        
        int cellCount = animeOperateArray.at(0).toInt();
        intervalValue = animeOperateArray.at(1).toInt();
        loopType = animeOperateArray.at(2).toInt();
        
        // 水平分割图像
        QList<QImage> imageArray;
        int cellWidth = bitmap.width() / cellCount;
        for (int i = 0; i < cellCount; i++) {
            QImage cellImg = bitmap.copy(i * cellWidth, 0, cellWidth, bitmap.height());
            QImage converted = transrateImageFromBitmap(cellImg, transmode);
            imageArray.append(converted);
        }
        
        return QImage(); // 返回动画对象需要特殊处理
    }
    
    // 普通图像转换
    return transrateImageFromBitmap(bitmap, transmode);
}

QImage TYResourceServer::getImage(const QString& path, bool transMode)
{
    return getImage(path, transMode, false);
}

QImage TYResourceServer::transrateImageFromBitmap(const QImage& bitmap, const QString& transmode)
{
    QImage result = bitmap;
    
    if (transmode == TYTRANSMODE_ALPHA) {
        result = ConvertImagePackedAlpha(bitmap);
    } else if (transmode == TYTRANSMODE_COPY) {
        // 保持原样
    } else if (transmode == TYTRANSMODE_LEFTTOP) {
        result = ConvertImageTransrateLeftTop(bitmap);
    } else if (transmode.startsWith(QLatin1String(":m"))) {
        // 遮罩模式
        QString maskPath = transmode.mid(2);
        QVariant maskData = getBitmap(maskPath);
        if (maskData.canConvert<QImage>()) {
            QImage maskImg = maskData.value<QImage>();
            result = ConvertImageWithMaskImage(bitmap, maskImg);
        }
    }
    
    return result;
}

QImage TYResourceServer::getImageFromString(const QString& str)
{
    // 变量替换
    QString processedStr = str; // TODO: 调用脚本引擎进行变量替换
    
    QString buffer;
    QList<QColor> colors;
    int i = 0;
    int len = processedStr.length();
    
    // 解析颜色
    while (i < len && processedStr.mid(i, 1) == QLatin1String("#")) {
        buffer = processedStr.mid(i, 7);
        colors.append(getColorWithHTMLFormat(buffer));
        i += 7;
    }
    
    // 获取文本属性
    TYStageManager* manager = TYStageManager::sharedManager();
    
    // 创建文本图像
    QList<QImage> images = TYStringImage::imagesWithString(
        processedStr.mid(i),
        manager->textAttDict(),
        colors,
        manager->textFontHeight(),
        manager->textFontWidth(),
        manager->textPitchx(),
        manager->isShadow()
    );
    
    return images.isEmpty() ? QImage() : images.at(0);
}

void TYResourceServer::addSpi(const QString& pluginName, const QString& extension)
{
    if (spiPluginDict.isEmpty()) {
        spiPluginDict = QMap<QString, QObject*>();
    }
    
    QString ext = extension.toLower();
    if (ext == QLatin1String("nbz")) {
        spiPluginDict[ext] = new TYNBZDecompresser(this);
    }
}

void TYResourceServer::addSoundPressPlugin(const QString& pluginName, const QString& extension)
{
    if (soundPressPluginDict.isEmpty()) {
        soundPressPluginDict = QMap<QString, QObject*>();
    }
    
    QString ext = extension.toLower();
    if (ext == QLatin1String("nbz")) {
        soundPressPluginDict[ext] = new TYNBZDecompresser(this);
    }
}

QByteArray TYResourceServer::getSoundData(const QString& path)
{
    if (path.isEmpty()) {
        return QByteArray();
    }
    
    QString convPath = winPathToUnix(path).toLower();
    
    // 使用解压插件
    if (!soundPressPluginDict.isEmpty()) {
        QString extension = QFileInfo(convPath).suffix();
        QObject* decompresser = soundPressPluginDict.value(extension);
        if (decompresser) {
            // 调用解码器
            for (TYArchiver* arc : ty_archivers) {
                QByteArray data = arc->unArchivedFile(convPath);
                if (!data.isEmpty())
                    return data;
            }
        }
    }
    
    return getData(convPath);
}

void TYResourceServer::addlog(const QString& filename)
{
    filelogDict[filename.toUpper()] = true;
}

void TYResourceServer::setDefaultTransMode(const QString& transmode)
{
    defaultTransmode = transmode;
}

bool TYResourceServer::fchk(const QString& filename) const
{
    return filelogDict.contains(filename.toUpper());
}

bool TYResourceServer::filelog(const QString& path)
{
    filelogDict.clear();
    filelog = true;
    
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return true;
    }
    
    // 读取文件数量
    QByteArray line = file.readLine();
    int fileNum = line.toInt();
    
    // 读取文件名
    for (int i = 0; i < fileNum; i++) {
        if (file.read(1) != "\"") break;
        
        QByteArray buf;
        char c;
        while (file.getChar(&c) && c != '"') {
            buf.append(c);
        }
        
        // XOR解码
        for (int j = 0; j < buf.size(); j++) {
            buf[j] = buf[j] ^ 132;
        }
        
        QString filename = TY_NSStringAddition::stringWithCSJISString(buf.constData());
        filelogDict[filename.toUpper()] = true;
    }
    
    file.close();
    return true;
}

bool TYResourceServer::saveLog(const QString& path)
{
    if (!filelog) {
        return true;
    }
    
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    // 写入文件数量
    int count = filelogDict.count();
    QString header = QString::number(count) + QLatin1String("\n");
    file.write(header.toUtf8());
    
    // 写入文件名
    for (auto it = filelogDict.constBegin(); it != filelogDict.constEnd(); ++it) {
        QByteArray sjisData = it.key().toLatin1(); // 简化处理
        for (int i = 0; i < sjisData.size(); i++) {
            sjisData[i] = sjisData[i] ^ 132;
        }
        file.write("\"" + sjisData + "\"");
    }
    
    file.close();
    return true;
}

void TYResourceServer::addEffectPattern(const QString& path)
{
    QVariant bmp = getBitmap(path);
    if (bmp.canConvert<QImage>()) {
        QImage bitmap = bmp.value<QImage>();
        TYEffectPatternMap* pattern = new TYEffectPatternMap(bitmap, this);
        if (pattern) {
            effectPatternDict[path] = pattern;
        }
    }
}

TYEffectPatternMap* TYResourceServer::getEffectPattern(const QString& path)
{
    if (effectPatternDict.contains(path)) {
        return effectPatternDict[path];
    }
    
    addEffectPattern(path);
    return effectPatternDict.value(path, nullptr);
}

void TYResourceServer::executeBundle(const QString& pathAndArg)
{
    QString bundlePath;
    QString argStr;
    
    // 解析路径和参数
    int slashIndex = pathAndArg.lastIndexOf(QLatin1Char('/'));
    if (slashIndex < 0) {
        bundlePath = pathAndArg;
    } else {
        bundlePath = pathAndArg.left(slashIndex);
        argStr = pathAndArg.mid(slashIndex + 1);
    }
    
    // 替换扩展名为bundle
    if (bundlePath.contains(QLatin1Char('.'))) {
        bundlePath = bundlePath.left(bundlePath.lastIndexOf(QLatin1Char('.'))) + QLatin1String(".bundle");
    }
    
    // 检查缓存
    if (executableBundles.contains(bundlePath)) {
        TYCCSProxy* proxy = TYCCSProxy::proxy();
        proxy->execPlugin(executableBundles[bundlePath], argStr);
        return;
    }
    
    // 加载bundle - Qt中需要使用QLibrary
    QString fullPath = getAppDirectory() + QDir::separator() + bundlePath;
    QLibrary library(fullPath);
    
    if (!library.load()) {
        qWarning() << "can't load bundle" << bundlePath;
        return;
    }
    
    // 获取主类 - 需要定义函数指针
    // 简化处理：存储库对象
    executableBundles[bundlePath] = nullptr; // TODO: 完善bundle加载
}
