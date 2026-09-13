//
//  TYStageManager.m
//  Tukuyomi
//
//  Created by toveta on Sun Aug 19 2001.
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

#include "TYStageManager.h"
#include "TYResourceServer.h"
//#import "TYMainController.h"
#include "TYImageUtil.h"
#include "TYMiscUtil.h"
#include "TYNScrSprite.h"
#include "TYCellImage.h"
#include "TYStringImage.h"
#include "TYSelectionManager.h"
#include "TYEffectGenerater.h"
#include "TYQuakeEffectGenerater.h"
#include "TYNew_QuakeEffectGenerater.h"
#include "TYNovelLayer.h"
#include "TYStandingChar.h"
#include "TYEnviroment.h"
#include "TYWaitCursor.h"
#include "TYExtraButtonManager.h"
#include "TYAutoModeController.h"
#include "TYArgment.h"
#include "TYConstStringArgment.h"
#include "TYBar.h"
#include "TYSoundController.h"
#include <QPainter>
#include <QTimer>
#include <QDebug>

// 全局变量
int VSCREEN_WIDTH = 640;
int VSCREEN_HEIGHT = 480;

QString CSEL_LABEL = QStringLiteral("*customsel");

// Helper functions
QRect TYVirtualScreenRect() {
    return QRect(0, 0, VSCREEN_WIDTH, VSCREEN_HEIGHT);
}

QSize TYVirtualScreenSize() {
    return QSize(VSCREEN_WIDTH, VSCREEN_HEIGHT);
}

QRect TYFromRect(const QPoint& drawPoint, const QSize& imageSize, const QSize& bgSize)
{
    QRect imageRect(0, 0, imageSize.width(), imageSize.height());
    QRect bgRect(-drawPoint.x(), -drawPoint.y(), bgSize.width(), bgSize.height());
    return imageRect.intersected(bgRect);
}

QRect TYFontRect(const QPoint& drawPoint, const QRect& fontRect, bool shadowed)
{
    return QRect(
        drawPoint.x() + fontRect.x(),
        drawPoint.y() + fontRect.y() - (shadowed ? SHADOW_TICKNESS : 0),
        fontRect.width() + (shadowed ? SHADOW_TICKNESS : 0),
        fontRect.height() + (shadowed ? SHADOW_TICKNESS : 0)
    );
}

// Static member
static TYStageManager* sharedManager = nullptr;

TYStageManager::TYStageManager(QObject* parent)
    : QObject(parent)
    , controller(nullptr)
    , engine(nullptr)
    , compositeLayer(QImage(VSCREEN_WIDTH, VSCREEN_HEIGHT, QImage::Format_ARGB32))
    , effectDefDict()
    , effectGenerater(nullptr)
    , afterEffectTarget(nullptr)
    , afterEffectSelector()
    , afterEffectObject(nullptr)
    , visualLayer(QImage(VSCREEN_WIDTH, VSCREEN_HEIGHT, QImage::Format_ARGB32))
    , bgImage(QImage(VSCREEN_WIDTH, VSCREEN_HEIGHT, QImage::Format_ARGB32))
    , bgPath()
    , bgRect(0, 0, VSCREEN_WIDTH, VSCREEN_HEIGHT)
    , underline(0)
    , leftCharImage(nullptr)
    , centerCharImage(nullptr)
    , rightCharImage(nullptr)
    , spriteArray()
    , humanz(25)
    , isUseHumanz(false)
    , isWindowback(false)
    , hideSpriteArray()
    , monocroSample()
    , nega(0)
    , novelLayer(nullptr)
    , textAttDict()
    , textShadowAttDict()
    , textFontName()
    , textFont(QFont())
    , displayFontSize(26, 26)
    , novelLayerOn(false)
    , eraseTextWindow(true)
    , textLeftOffset(8)
    , textTopOffset(16)
    , textColumn(23)
    , textRow(16)
    , textFontWidth(26)
    , textFontHeight(26)
    , textPitchx(0)
    , textPitchy(2)
    , textSpeed(&userTextSpeed)
    , userTextSpeed(20)
    , scriptTextSpeed(20)
    , defaultSpeed()
    , selectedSpeedTag(0)
    , textBold(true)
    , textShadowed(true)
    , locateX(0)
    , textWindow()
    , textWindowPath()
    , textWindowRect(0, 0, VSCREEN_WIDTH, VSCREEN_HEIGHT)
    , tpStatus(TYTPOff)
    , waitNewPage(false)
    , forceClickWait(false)
    , forceWaitNewPage(false)
    , invalidNewLine(false)
    , clickStrSet()
    , clickStrUnderLimit(0)
    , hyphenationSet()
    , nowColumn(0)
    , nowRow(0)
    , nowStorageLength(0)
    , nowStorageOffset(0)
    , buttonManager(nullptr)
    , beforeSelectButtonID(-1)
    , buttonTargetValue()
    , releaseButtonAfterSelect(false)
    , isDrawButton(false)
    , btnTime(0)
    , btnStartDate()
    , selectionManager(nullptr)
    , beforeSelectIndex(-1)
    , selectAction()
    , selectColor(Qt::white)
    , unSelectColor(QColor(255 - 99, 255 - 99, 255 - 99))
    , skipStatus(TYNoSkip)
    , automodeWait(0)
    , automodeWaitType(0)
    , waitCountOfChar(0)
    , playingSoundFlg(false)
    , waitFinishSound(false)
    , waitFinishSoundOnBtnTime2(false)
    , clickWaitCursor(nullptr)
    , pageWaitCursor(nullptr)
    , currentCursor(nullptr)
    , linepage(false)
    , linepageWait(false)
    , textgosubLabel()
    , isPage(false)
    , cselInfo()
    , cselImage()
    , lookbackQueue()
    , barDict()
{
    if (!sharedManager) {
        sharedManager = this;
    }
    
    // 初始化特效定义字典
    effectDefDict[-2] = TYEffectDefinitionValue(TYEffectInstant, 0, nullptr);
    effectDefDict[0] = TYEffectDefinitionValue(TYEffectCached, 0, nullptr);
    effectDefDict[1] = TYEffectDefinitionValue(TYEffectInstant, 0, nullptr);
    
    // 初始化默认速度
    defaultSpeed = {40, 20, 10};
    
    // 初始化字体
    textFont = QFont(textFontName.isEmpty() ? QStringLiteral("MS Gothic") : textFontName, textFontHeight);
    textFontColor = Qt::white;
    
    // 初始化文字属性
    makeAttributeDict();
    
    // 初始化文本窗口
    textWindowRect = TYVirtualScreenRect();
    
    // 初始化NovelLayer
    novelLayer = new TYNovelLayer(this);
    novelLayer->setManager(this);
    
    // 初始化按钮管理器
    buttonManager = new TYExtraButtonManager(this, nullptr);
    
    // 初始化选择颜色
    selectColor = Qt::white;
    unSelectColor = QColor(156, 156, 156);
    
    // 初始化等待光标
    clickWaitCursor = new TYWaitCursor(QStringLiteral(":l/3,160,2;cursor0.bmp"), QPoint(0, 0), false, this);
    pageWaitCursor = new TYWaitCursor(QStringLiteral(":l/3,160,2;cursor1.bmp"), QPoint(0, 0), false, this);
    
    // 连接信号
    connect(TYSoundController::instance(), &TYSoundController::soundStarted,
            this, &TYStageManager::startWaveSound);
    connect(TYSoundController::instance(), &TYSoundController::soundFinished,
            this, &TYStageManager::stopWaveSound);
}

TYStageManager::~TYStageManager()
{
}

TYStageManager* TYStageManager::sharedManager()
{
    return sharedManager;
}

void TYStageManager::setModeSVGA()
{
    VSCREEN_WIDTH = 800;
    VSCREEN_HEIGHT = 600;
}

void TYStageManager::setController(QObject* aController, QObject* aEngine)
{
    controller = aController;
    engine = aEngine;
}

// 保存数据编码
QVariant TYStageManager::encodeWithSaveData()
{
    QVariantMap aDict;
    
    // NovelLayer
    if (novelLayer) {
        aDict[QStringLiteral("NovelLayer")] = novelLayer->toVariant();
    }
    
    // 文字窗口信息
    QVariantList textInfo = {
        textLeftOffset,
        textTopOffset,
        textColumn,
        textRow,
        textFontWidth,
        textFontHeight,
        textPitchx,
        textPitchy,
        userTextSpeed,
        textBold,
        textShadowed,
        textWindowPath,
        textWindowRect.x(),
        textWindowRect.y(),
        textWindowRect.width(),
        textWindowRect.height()
    };
    aDict[QStringLiteral("TextWindow")] = textInfo;
    
    // 速度模式
    aDict[QStringLiteral("SpeedMode")] = (textSpeed == &userTextSpeed) ? TYUserSpeedMode : TYScriptSpeedMode;
    
    // 打印信息
    QVariantList printInfo = {
        textQueue,
        nowStorageOffset,
        nowRow,
        nowColumn,
        waitNewPage,
        invalidNewLine
    };
    aDict[QStringLiteral("PrintingInfo")] = printInfo;
    aDict[QStringLiteral("IsPage")] = isPage;
    
    // 显示层可见性
    aDict[QStringLiteral("NovelLayerVisible")] = novelLayerOn;
    
    // 背景图像
    if (!bgPath.isEmpty()) {
        aDict[QStringLiteral("BGImagePath")] = bgPath;
    }
    
    // 角色图像
    if (leftCharImage) {
        aDict[QStringLiteral("LeftChar")] = leftCharImage->toVariant();
    }
    if (centerCharImage) {
        aDict[QStringLiteral("CenterChar")] = centerCharImage->toVariant();
    }
    if (rightCharImage) {
        aDict[QStringLiteral("RightChar")] = rightCharImage->toVariant();
    }
    
    // 下划线
    aDict[QStringLiteral("Underline")] = underline;
    
    // 黑白/反色
    if (!monocroSample.isEmpty()) {
        aDict[QStringLiteral("MonocroSample")] = monocroSample;
    }
    aDict[QStringLiteral("Nega")] = nega;
    
    // 光标信息
    if (currentCursor && tpStatus == TYTPClickWait) {
        if (currentCursor == clickWaitCursor) {
            aDict[QStringLiteral("ClickWaitType")] = TYNormalClickWaitType;
        } else if (currentCursor == pageWaitCursor) {
            aDict[QStringLiteral("ClickWaitType")] = TYPageClickWaitType;
        }
    }
    
    // 自定义选择信息
    if (!cselInfo.isEmpty()) {
        aDict[QStringLiteral("CustomSelectInfo")] = cselInfo;
    }
    
    // 文字窗口清除
    aDict[QStringLiteral("EraseTextWindow")] = eraseTextWindow;
    
    // 回溯队列
    if (!lookbackQueue.isEmpty()) {
        aDict[QStringLiteral("LookbackBuffer")] = lookbackQueue;
    }
    
    return aDict;
}

// 保存数据解码
void TYStageManager::decodeWithSaveData(const QVariant& aObject)
{
    QVariantMap aDict = aObject.toMap();
    
    // NovelLayer
    if (aDict.contains(QStringLiteral("NovelLayer"))) {
        if (novelLayer) {
            novelLayer->fromVariant(aDict[QStringLiteral("NovelLayer")]);
        }
    }
    
    // 文字窗口信息
    if (aDict.contains(QStringLiteral("TextWindow"))) {
        QVariantList textInfo = aDict[QStringLiteral("TextWindow")].toList();
        if (textInfo.count() >= 15) {
            textLeftOffset = textInfo[0].toInt();
            textTopOffset = textInfo[1].toInt();
            textColumn = textInfo[2].toInt();
            textRow = textInfo[3].toInt();
            textFontWidth = textInfo[4].toInt();
            textFontHeight = textInfo[5].toInt();
            textPitchx = textInfo[6].toInt();
            textPitchy = textInfo[7].toInt();
            userTextSpeed = textInfo[8].toInt();
            textBold = textInfo[9].toBool();
            textShadowed = textInfo[10].toBool();
            textWindowPath = textInfo[11].toString();
            textWindowRect.setRect(
                textInfo[12].toInt(),
                textInfo[13].toInt(),
                textInfo[14].toInt(),
                textInfo[15].toInt()
            );
            makeAttributeDict();
        }
    }
    
    // 速度模式
    if (aDict.contains(QStringLiteral("SpeedMode"))) {
        textSpeed = (aDict[QStringLiteral("SpeedMode")].toInt() == TYScriptSpeedMode) 
                    ? &scriptTextSpeed : &userTextSpeed;
    }
    
    // 打印信息
    if (aDict.contains(QStringLiteral("PrintingInfo"))) {
        QVariantList printInfo = aDict[QStringLiteral("PrintingInfo")].toList();
        if (printInfo.count() >= 6) {
            textQueue = printInfo[0].toString();
            nowStorageLength = textQueue.length();
            nowStorageOffset = printInfo[1].toInt();
            nowRow = printInfo[2].toInt();
            nowColumn = printInfo[3].toInt();
            waitNewPage = printInfo[4].toBool();
            invalidNewLine = printInfo[5].toBool();
            waitCountOfChar = nowRow * textColumn + nowColumn;
        }
    }
    
    // 显示层可见性
    if (aDict.contains(QStringLiteral("NovelLayerVisible"))) {
        novelLayerOn = aDict[QStringLiteral("NovelLayerVisible")].toBool();
    }
    
    // 背景图像
    QString loadBgPath = aDict.contains(QStringLiteral("BGImagePath")) 
                        ? aDict[QStringLiteral("BGImagePath")].toString() 
                        : QStringLiteral("black");
    loadBGImage(loadBgPath);
    
    // 重置强制等待
    forceWaitNewPage = false;
    forceClickWait = false;
    
    // 角色图像
    if (aDict.contains(QStringLiteral("LeftChar")) && leftCharImage) {
        leftCharImage->fromVariant(aDict[QStringLiteral("LeftChar")]);
    }
    if (aDict.contains(QStringLiteral("CenterChar")) && centerCharImage) {
        centerCharImage->fromVariant(aDict[QStringLiteral("CenterChar")]);
    }
    if (aDict.contains(QStringLiteral("RightChar")) && rightCharImage) {
        rightCharImage->fromVariant(aDict[QStringLiteral("RightChar")]);
    }
    
    // 更新舞台
    updateStage();
}

// 更新舞台
void TYStageManager::updateStage()
{
    // 清空复合层
    compositeLayer.fill(Qt::black);
    
    QPainter painter(&compositeLayer);
    
    // 绘制背景
    painter.drawImage(bgRect, bgImage, bgImage.rect());
    
    // 绘制角色
    if (leftCharImage && leftCharImage->isVisible()) {
        painter.drawImage(leftCharImage->rect(), leftCharImage->image());
    }
    if (centerCharImage && centerCharImage->isVisible()) {
        painter.drawImage(centerCharImage->rect(), centerCharImage->image());
    }
    if (rightCharImage && rightCharImage->isVisible()) {
        painter.drawImage(rightCharImage->rect(), rightCharImage->image());
    }
    
    // 绘制Sprite
    for (TYNScrSprite* sprite : spriteArray) {
        if (sprite && sprite->isVisible()) {
            painter.drawImage(sprite->rect(), sprite->image());
        }
    }
    
    // 绘制文字窗口
    if (novelLayerOn && !eraseTextWindow) {
        painter.drawImage(textWindowRect, *novelLayer->toImage(), novelLayer->rect());
    }
    
    painter.end();
    
    // 发送更新信号
    emit stageUpdated();
}

void TYStageManager::updateStageOnlyButton()
{
    // 仅更新按钮部分
    emit stageUpdated();
}

QImage TYStageManager::visualLayerImage() const
{
    return visualLayer;
}

QImage TYStageManager::compositeLayerImage() const
{
    return compositeLayer;
}

// 加载背景图像
void TYStageManager::loadBGImage(const QString& aPath)
{
    if (bgPath == aPath) return;
    
    bgPath = aPath;
    
    if (aPath.isEmpty() || aPath == QStringLiteral("black")) {
        bgImage.fill(Qt::black);
    } else {
        QImage img = TYResourceServer::sharedServer()->getImage(aPath, false);
        if (!img.isNull()) {
            bgImage = img.scaled(VSCREEN_WIDTH, VSCREEN_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
    }
    
    bgRect = QRect(0, 0, bgImage.width(), bgImage.height());
    
    // 添加到回溯队列
    lookbackQueue.append(bgImage);
    if (lookbackQueue.size() > 100) {
        lookbackQueue.removeFirst();
    }
}

void TYStageManager::loadTextWindow(const QString& path)
{
    textWindowPath = path;
    // 加载文字窗口图像
    if (!path.isEmpty() && path != QStringLiteral("#999999")) {
        textWindow = TYResourceServer::sharedServer()->getImage(path, true);
    }
}

void TYStageManager::changeVisualLayer(int effectNum)
{
    // 视觉层变化效果
    TYEffectDefinitionValue effectDef = effectDefDict.value(effectNum);
    
    switch (effectDef.type) {
    case TYEffectInstant:
        updateStage();
        break;
    case TYEffectCached:
        // 缓存效果
        break;
    default:
        updateStage();
        break;
    }
}

void TYStageManager::flushNovelLayer()
{
    novelLayerOn = true;
    updateStage();
}

void TYStageManager::startEraseTextWindow(int effectNum)
{
    eraseTextWindow = true;
    updateStage();
}

// 添加Sprite
void TYStageManager::addSprite(const QVariantList& argments, bool visible)
{
    Q_UNUSED(argments);
    Q_UNUSED(visible);
    // 实现Sprite添加逻辑
}

// 开始选择
void TYStageManager::startSelect(const QStringList& messages, const QVariantList& responds)
{
    Q_UNUSED(messages);
    Q_UNUSED(responds);
    // 实现选择逻辑
}

// 重置游戏
void TYStageManager::resetGame()
{
    textQueue.clear();
    nowRow = 0;
    nowColumn = 0;
    waitNewPage = false;
    eraseTextWindow = true;
    humanz = 25;
    
    // 清除所有Sprite
    spriteArray.clear();
    
    // 清除角色
    if (leftCharImage) leftCharImage->setVisible(false);
    if (centerCharImage) centerCharImage->setVisible(false);
    if (rightCharImage) rightCharImage->setVisible(false);
    
    // 重置回溯队列
    lookbackQueue.clear();
    
    // 更新舞台
    updateStage();
}

int TYStageManager::status() const
{
    return tpStatus;
}

void TYStageManager::setStatus(int newStatus)
{
    tpStatus = static_cast<enum TYTPStatus>(newStatus);
}

void TYStageManager::printing()
{
    // 文字打印逻辑
    tpStatus = TYTPPrinting;
}

void TYStageManager::resumePrinting()
{
    // 恢复打印
    tpStatus = TYTPPrinting;
}

// 设置跳过状态
void TYStageManager::setSkipStatus(int aStatus)
{
    skipStatus = aStatus;
}

int TYStageManager::skipStatusValue() const
{
    return skipStatus;
}

void TYStageManager::setClickWait(bool pageWait)
{
    waitNewPage = pageWait;
    tpStatus = pageWait ? TYTPTimeWait : TYTPClickWait;
}

// 文本打印
void TYStageManager::textPrint(const QString& aString)
{
    textQueue.append(aString);
    nowStorageLength = textQueue.length();
    nowStorageOffset = 0;
    nowRow = 0;
    nowColumn = 0;
    
    printing();
    updateStage();
}

void TYStageManager::drawTextWindow()
{
    // 绘制文字窗口
    if (!textWindow.isNull()) {
        // 绘制窗口背景
    }
}

QRect TYStageManager::getTextWindowRect() const
{
    return textWindowRect;
}

void TYStageManager::drawChar(const QString& charStr, const QColor& color, int column, int row)
{
    Q_UNUSED(charStr);
    Q_UNUSED(color);
    Q_UNUSED(column);
    Q_UNUSED(row);
}

void TYStageManager::drawAttStr(const QString& str, int column, int row)
{
    Q_UNUSED(str);
    Q_UNUSED(column);
    Q_UNUSED(row);
}

QPoint TYStageManager::drawPoint() const
{
    return QPoint(
        textWindowRect.x() + textLeftOffset + locateX + nowColumn * (textFontWidth + textPitchx),
        textWindowRect.y() + textTopOffset + nowRow * (textFontHeight + textPitchy)
    );
}

// 创建属性字典
void TYStageManager::makeAttributeDict()
{
    textAttDict[QStringLiteral("font")] = textFont;
    textAttDict[QStringLiteral("color")] = textFontColor;
    textAttDict[QStringLiteral("bold")] = textBold;
    textAttDict[QStringLiteral("shadow")] = textShadowed;
    
    // 创建阴影属性
    textShadowAttDict = textAttDict;
    textShadowAttDict[QStringLiteral("shadowColor")] = QColor(0, 0, 0, 128);
}

QVariant TYStageManager::textAttDict() const
{
    return textAttDict;
}

int TYStageManager::textFontWidthValue() const
{
    return textFontWidth;
}

int TYStageManager::textFontHeightValue() const
{
    return textFontHeight;
}

int TYStageManager::textPitchxValue() const
{
    return textPitchx;
}

bool TYStageManager::isShadow() const
{
    return textShadowed;
}

// 字体改变
void TYStageManager::changeFont(const QFont& fontManager)
{
    textFont = fontManager;
    makeAttributeDict();
}

// 文字速度改变
void TYStageManager::changeTextSpeed(int index)
{
    if (index >= 0 && index < defaultSpeed.size()) {
        userTextSpeed = defaultSpeed[index];
        selectedSpeedTag = index;
    }
}

// 自动模式设置
void TYStageManager::setAutoMode(int speed, int waitType, int wait)
{
    userTextSpeed = speed;
    automodeWait = wait;
    automodeWaitType = waitType;
}

int TYStageManager::userTextSpeedValue() const
{
    return userTextSpeed;
}

// 按钮等待
void TYStageManager::startButtonWait(const QVariantList& argments, bool releaseAfterSelect)
{
    Q_UNUSED(argments);
    releaseButtonAfterSelect = releaseAfterSelect;
    tpStatus = TYButtonWaitStatus;
}

void TYStageManager::changeSelectedButton(unsigned int btnIndex)
{
    Q_UNUSED(btnIndex);
}

void TYStageManager::responseButton(int result)
{
    Q_UNUSED(result);
    // 按钮响应处理
}

void TYStageManager::changeSelection(int index)
{
    beforeSelectIndex = index;
}

void TYStageManager::responseSelection()
{
    // 选择响应处理
}

void TYStageManager::makeCselImage()
{
    // 创建自定义选择图像
}

// Sprite操作
QRect TYStageManager::rectOfSprite(int idNo) const
{
    for (TYNScrSprite* sprite : spriteArray) {
        if (sprite && sprite->spriteId() == idNo) {
            return sprite->rect();
        }
    }
    return QRect();
}

void TYStageManager::setSprite(int idNo, bool visible)
{
    for (TYNScrSprite* sprite : spriteArray) {
        if (sprite && sprite->spriteId() == idNo) {
            sprite->setVisible(visible);
            break;
        }
    }
}

void TYStageManager::setSprite(int idNo, int cellNo)
{
    for (TYNScrSprite* sprite : spriteArray) {
        if (sprite && sprite->spriteId() == idNo) {
            sprite->setCell(cellNo);
            break;
        }
    }
}

// 特效相关
void TYStageManager::changeEffectionImage(const QImage& drawImage)
{
    Q_UNUSED(drawImage);
}

void TYStageManager::effectFinished()
{
    // 特效完成处理
}

// 通知处理
void TYStageManager::updateSprites()
{
    updateStage();
}

void TYStageManager::startWaveSound()
{
    playingSoundFlg = true;
    if (waitFinishSound) {
        // 等待声音完成
    }
}

void TYStageManager::stopWaveSound()
{
    playingSoundFlg = false;
    if (waitFinishSoundOnBtnTime2) {
        // 按钮时间2的声音停止
    }
}

// 系统模式
void TYStageManager::enterSystemMode()
{
    // 进入系统模式
}

void TYStageManager::exitSystemModeResumeStatus(bool aBool)
{
    Q_UNUSED(aBool);
    // 退出系统模式
}

bool TYStageManager::isPossibleEnterSystemMode() const
{
    return tpStatus == TYPrinting || tpStatus == TYTPClickWait;
}

// 选择动作
void TYStageManager::select_action()
{
    switch (tpStatus) {
    case TYTPPrinting:
        // 完成当前打印
        break;
    case TYTPClickWait:
        tpStatus = TYTPOff;
        break;
    default:
        break;
    }
}

void TYStageManager::cancel_action(QObject* sender)
{
    Q_UNUSED(sender);
}

// 回溯队列
QVariantList TYStageManager::getLookbackQueue() const
{
    return lookbackQueue;
}

// 自动模式选择动作
void TYStageManager::automode_select_action()
{
    // 自动模式下的选择动作
}

// 特效生成器相关
void TYStageManager::setEffectGenerater(TYEffectGenerater* generater)
{
    effectGenerater = generater;
}

// 获取特效定义
TYEffectDefinitionValue TYStageManager::getEffectDefinition(int effectNo) const
{
    return effectDefDict.value(effectNo, TYEffectDefinitionValue(TYEffectInstant, 0, nullptr));
}

// 文字清除
void TYStageManager::ty_textclear(const QVariantList& argments)
{
    Q_UNUSED(argments);
    textQueue.clear();
    nowRow = 0;
    nowColumn = 0;
    novelLayerOn = false;
    updateStage();
}

// 回溯刷新
void TYStageManager::ty_lookbackflush(const QVariantList& argments)
{
    Q_UNUSED(argments);
    lookbackQueue.clear();
}

// 特效文本
void TYStageManager::ty_spstr(const QVariantList& argments)
{
    Q_UNUSED(argments);
}

// 选择
void TYStageManager::ty_select(const QVariantList& argments)
{
    Q_UNUSED(argments);
}

// 自动模式选择
void TYStageManager::automode_select_action_impl()
{
    automode_select_action();
}
