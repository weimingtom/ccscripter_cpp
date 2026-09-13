//
//  TYMainController.m
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

#include "TYMainController.h"
#include "TYStageManager.h"
#include "TYScriptEngine.h"
#include "TYVisualNovelView.h"
#include "TYSoundController.h"
#include "TYResourceServer.h"
#include "TYMiscUtil.h"
#include "TYSaveLoadManager.h"
#include "TYEnviroment.h"
#include "TYAudioPathController.h"
#include "TYFullScreenWindow.h"
#include "TYFullScreenContentView.h"
#include "TYAutoModeController.h"
#include "TYExtVolumeController.h"
#include "TYPreferencePanelController.h"
#include "TYWave2Aiff.h"
#include "TYEraseModeView.h"
#include "TYRootMenuView.h"
#include "TYImageUtil.h"
#include "TYYesNoMenuView.h"
#include "TYSaveLoadMenuView.h"
#include "TYLookbackView.h"
#include "TYCellImage.h"
#include "TYScreenManager.h"
#include "TYMovieControllerView.h"
#include "TYCCSProxy.h"
#include "TYArgment.h"
#include "TYDigitArgment.h"

// Static member
TYMainController* TYMainController::controller()
{
    static TYMainController instance;
    return &instance;
}

// Constants implementation
const QString TYRMenuFontSize = "TYRMenuFontSize";
const QString TYRMenuFontPitchSize = "TYRMenuFontPitchSize";
const QString TYRMenuShadowed = "TYRMenuShadowed";
const QString TYRMenuBold = "TYRMenuBold";
const QString TYRMenuBGColor = "TYRMenuBGColor";
const QString TYRMenuSelectColor = "TYRMenuSelectColor";
const QString TYRMenuNoSelectColor = "TYRMenuNoSelectColor";
const QString TYRMenuNoDataColor = "TYRMenuNoDataColor";

const QString TYVoicePlayNotification = "TYVoicePlayNotification";

// Constructor
TYMainController::TYMainController(QObject* parent)
    : QObject(parent)
    , m_mainMenu(nullptr)
    , m_mainView(nullptr)
    , m_targetWindow(nullptr)
    , m_defaultContentView(nullptr)
    , m_mainWindow(nullptr)
    , m_fullScreenWindow(nullptr)
    , m_speedMenu(nullptr)
    , m_skipMenu(nullptr)
    , m_nscrMenu(nullptr)
    , m_contextMenu(nullptr)
    , m_saveLoadController(nullptr)
    , m_bgmView(nullptr)
    , m_qtMovie(nullptr)
    , m_gMyTimer(nullptr)
    , m_bgmForceSave(false)
    , m_isFullScreenMode(false)
    , m_scriptEngine(nullptr)
    , m_stageManager(nullptr)
    , m_autoclickTimer(0)
    , m_tyStatus(TYScriptRunningStatus)
    , m_trapSelectAction(false)
    , m_audioPathController(nullptr)
    , m_bgmVolume(1.0f)
    , m_volumeController(nullptr)
    , m_defBGMVol(1.0f)
    , m_defVoiceVol(1.0f)
    , m_defSEVol(1.0f)
    , m_waveController(nullptr)
    , m_mode_ext(false)
    , m_enable_automode(false)
    , m_automode_time(0)
    , m_isModal(false)
    , m_movieView(nullptr)
    , m_ctrlView(nullptr)
    , m_menuModeView(nullptr)
    , m_isSystemMode(false)
{
    // Initialize direct wave controllers
    for (int i = 0; i < DIRECT_WAVE_MAX; i++) {
        m_directWaveController[i] = nullptr;
    }
}

TYMainController::~TYMainController()
{
    // Cleanup
    if (m_gMyTimer) {
        m_gMyTimer->stop();
        delete m_gMyTimer;
    }
    if (m_saveLoadController) {
        delete m_saveLoadController;
    }
    if (m_audioPathController) {
        delete m_audioPathController;
    }
    if (m_volumeController) {
        delete m_volumeController;
    }
    if (m_waveController) {
        delete m_waveController;
    }
    for (int i = 0; i < DIRECT_WAVE_MAX; i++) {
        if (m_directWaveController[i]) {
            delete m_directWaveController[i];
        }
    }
}

void TYMainController::applicationDidFinishLaunching()
{
    // Load and initialize script engine
    m_scriptEngine = new TYScriptEngine(getNScrRootDirectory());
    if (!m_scriptEngine) {
        if (!TYEnviroment::boolForKey(TYDisableSelectScriptEnviroment)) {
            // Open panel for directory selection - simplified for Qt
            QString str;
            // Note: In Qt, would use QFileDialog::getExistingDirectory
            // For now, assume directory is already set
        }

        if (!m_scriptEngine) {
            alertAndTerminate("Error", "Script file not found");
        }
    }

    // Set target for main view
    if (m_mainView) {
        m_mainView->setTarget(this);
        m_mainView->setFrame(TYVirtualScreenRect());
    }

    // Set content size and accept mouse moved events
    if (m_mainWindow) {
        m_mainWindow->setFixedSize(QSize(VSCREEN_WIDTH, VSCREEN_HEIGHT));
        m_mainWindow->setMouseTracking(true);
    }

    // Center window
    if (m_mainWindow) {
        m_mainWindow->move(
            QApplication::primaryScreen()->geometry().center() - m_mainWindow->rect().center()
        );
    }

    // Create save/load controller
    m_saveLoadController = new TYSaveLoadManager();
    m_saveLoadController->setNumber(9);

    // Set audio path
    setAudioPath(TYEnviroment::objectForKey(TYAudioFilePathEnviroment).toString());

    // Check for SVGA mode
    if (m_scriptEngine && m_scriptEngine->isModeSVGA()) {
        TYStageManager::setModeSVGA();
        if (m_mainWindow) {
            m_mainWindow->setFixedSize(TYVirtualScreenSize());
        }
    }

    // Initialize stage manager
    m_stageManager = new TYStageManager();

    // Set controller and engine for each other
    if (m_scriptEngine) {
        m_scriptEngine->setController(this);
        m_scriptEngine->setStageManager(m_stageManager);
    }
    if (m_stageManager) {
        m_stageManager->setController(this);
        m_stageManager->setEngine(m_scriptEngine);
    }

    // Set default volumes
    m_defBGMVol = 1.0f;
    m_defVoiceVol = 1.0f;
    m_defSEVol = 1.0f;

    // Load volume settings
    QVariant aObj = TYEnviroment::objectForKey(TYBGMVolumeEnviroment);
    if (aObj.isValid()) {
        m_bgmVolume = aObj.toFloat();
    } else {
        m_bgmVolume = 1.0f;
    }

    aObj = TYEnviroment::objectForKey(TYVoiceVolumeEnviroment);
    if (aObj.isValid()) {
        m_voiceVol = aObj.toFloat() * 100 - 100;
    }

    aObj = TYEnviroment::objectForKey(TYSEVolumeEnviroment);
    if (aObj.isValid()) {
        m_seVol = aObj.toFloat() * 100 - 100;
    }

    // Initialize status
    m_tyStatus = TYScriptRunningStatus;

    // Initialize right-click menu
    m_rightMenuLayout[TYRMenuSelectColor] = QColor(Qt::white);
    m_rightMenuLayout[TYRMenuNoSelectColor] = QColor(Qt::gray);
    m_rightMenuLayout[TYRMenuNoDataColor] = QColor(Qt::gray);
    m_rightMenuLayout[TYRMenuBGColor] = getTextWindowColorWithHTMLFormat("#999999");

    // Call menu set window with default values
    QVector<QVariant> args;
    args << "menusetwindow" << 26 << 26 << 0 << 2 << 0 << 1 << "#999999";
    ty_menusetwindow(args);

    // Start in full screen mode if enabled
    if (TYEnviroment::boolForKey(TYFullScreenModeEnviroment)) {
        QTimer::singleShot(0, this, SLOT(fullScreenMode()));
    }

    // Start script
    if (m_scriptEngine) {
        QTimer::singleShot(0, m_scriptEngine, SLOT(runScript()));
    }
}

int TYMainController::applicationShouldTerminate()
{
    if (m_scriptEngine) {
        m_scriptEngine->saveExternalData();
    }

    TYResourceServer::sharedServer()->saveLog(
        getNScrRootDirectory() + "/" + FILELOG_FILENAME
    );

    // Save window position
    if (m_mainWindow) {
        QPoint windowPoint = m_mainWindow->pos();
        TYEnviroment::setObject(windowPoint.x(), TYWindowPointXEnviroment);
        TYEnviroment::setObject(windowPoint.y(), TYWindowPointYEnviroment);
    }

    TYEnviroment::setObject(m_isFullScreenMode ? true : false, TYFullScreenModeEnviroment);
    TYEnviroment::save();

    // Remove temp file
    if (!m_tmpPath.isEmpty()) {
        QFile::remove(m_tmpPath);
    }

    TYWave2AiffCleanUp();

    if (m_isFullScreenMode && TYEnviroment::boolForKey(TYHideOtherFullScreenEnviroment)) {
        windowMode(nullptr);
        TYEnviroment::setObject(true, TYFullScreenModeEnviroment);
    }

    return 0; // QMainWindow requests exit
}

void TYMainController::alertAndTerminate(const QString& title, const QString& msg)
{
    QMessageBox::critical(nullptr, title, msg, QMessageBox::Ok);
    QCoreApplication::quit();
}

void TYMainController::disableContextMenu()
{
    if (m_mainView) {
        m_mainView->setContextMenuPolicy(Qt::NoContextMenu);
    }
}

void TYMainController::setAudioPath(const QString& aPath)
{
    QString path = aPath;
    if (path.startsWith("~")) {
        path = QDir::homePath() + path.mid(1);
    }

    if (!QDir(path).isAbsolute()) {
        path = getNScrRootDirectory() + "/" + path;
    }

    m_audioSearchPath = path;
}

void TYMainController::setStatus(int aStatus)
{
    m_tyStatus = aStatus;
    if ((aStatus == TYClickWaitStatus) && m_autoclickTimer) {
        QTimer::singleShot(m_autoclickTimer / 1000.0, this, SLOT(select_action()));
    }
}

int TYMainController::status()
{
    return m_tyStatus;
}

void TYMainController::saveLocalData(int number)
{
    QMap<QString, QVariant> aDict;

    // Get current date
    QDateTime cDate = QDateTime::currentDateTime();
    aDict[TYMakeDateSaveData] = cDate;
    aDict[TYNumberSaveData] = number;
    aDict[TYVersionSaveData] = TYSaveDataVersion;

    // Determine save data type based on status
    if (status() == TYScriptRunningStatus) {
        aDict[TYDataTypeSaveData] = TYScriptingSaveData;
    } else if (status() == TYPrinting && m_stageManager && m_stageManager->status() == TYTPClickWait) {
        aDict[TYDataTypeSaveData] = TYPrintingWaitSaveData;
    } else {
        aDict[TYDataTypeSaveData] = TYScriptingSaveData;
    }

    // Encode controller data
    QMap<QString, QVariant> objDict = encodeWithSaveData();
    aDict.unite(objDict);

    // Encode script engine data
    if (m_scriptEngine) {
        objDict = m_scriptEngine->encodeWithSaveData();
        aDict.unite(objDict);
    }

    // Encode stage manager data
    if (m_stageManager) {
        objDict = m_stageManager->encodeWithSaveData();
        aDict.unite(objDict);
    }

    // Save to file
    QString filename = QString(SAVEDATA_NAME).arg(number);
    QString path = getNScrRootDirectory() + "/" + filename;

    // Save using archiver
    QFile file(path);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << aDict;
        file.close();
    }

    // Reset date in controller
    if (m_saveLoadController) {
        m_saveLoadController->resetDateWithNumber(number);
    }

    // Save external data
    if (m_scriptEngine) {
        m_scriptEngine->saveExternalData();
    }
    TYResourceServer::sharedServer()->saveLog(
        getNScrRootDirectory() + "/" + FILELOG_FILENAME
    );
}

bool TYMainController::loadLocalData(int number)
{
    QString filename = QString(SAVEDATA_NAME).arg(number);
    QString path = getNScrRootDirectory() + "/" + filename;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QDataStream in(&file);
    QMap<QString, QVariant> aDict;
    in >> aDict;
    file.close();

    if (aDict.isEmpty()) {
        return false;
    }

    // Decode data
    decodeWithSaveData(aDict);
    if (m_scriptEngine) {
        m_scriptEngine->decodeWithSaveData(aDict);
    }
    if (m_stageManager) {
        m_stageManager->decodeWithSaveData(aDict);
    }

    if (status() == TYScriptRunningStatus) {
        if (m_scriptEngine) {
            m_scriptEngine->setBreakRun(true);
        }
    }

    int scriptStatus = aDict[TYDataTypeSaveData].toInt();
    if (scriptStatus == TYScriptingSaveData) {
        setStatus(TYScriptRunningStatus);
        if (m_scriptEngine) {
            QTimer::singleShot(0, m_scriptEngine, SLOT(runScript()));
        }
    } else {
        setStatus(TYPrinting);
        if (m_stageManager) {
            m_stageManager->setStatus(TYTPClickWait);
        }
    }

    return true;
}

QMap<QString, QVariant> TYMainController::encodeWithSaveData()
{
    QMap<QString, QVariant> dict;

    if (m_waveController) {
        QString wavePath = m_waveController->soundPathIfNeedPlayingAtLoad();
        if (!wavePath.isEmpty()) {
            dict[TYWavePathSaveData] = wavePath;
        }
    }

    if (!m_bgmPath.isEmpty()) {
        // Note: In Qt, check loop mode differently
        dict[TYBGMPathSaveData] = m_bgmPath;
    }

    dict[TYAutoClickTimerSaveData] = m_autoclickTimer;

    return dict;
}

void TYMainController::decodeWithSaveData(const QMap<QString, QVariant>& aObject)
{
    // Stop BGM if playing
    if (m_bgmView) {
        // Stop BGM view
    }

    if (!m_bgmPath.isEmpty()) {
        m_bgmPath.clear();
    }

    // Load BGM path
    if (aObject.contains(TYBGMPathSaveData)) {
        QString temp = aObject[TYBGMPathSaveData].toString();
        bool temp2 = aObject[TYBGMNoLoopSaveData].toBool();
        playSound(temp, !temp2, temp2);
    }

    // Clear wave controller
    if (m_waveController) {
        delete m_waveController;
        m_waveController = nullptr;
    }

    // Load wave path
    if (aObject.contains(TYWavePathSaveData)) {
        QString temp = aObject[TYWavePathSaveData].toString();
        m_waveController = new TYSoundController(temp, true);
        m_waveController->play();
    }

    // Load autoclick timer
    if (aObject.contains(TYAutoClickTimerSaveData)) {
        m_autoclickTimer = aObject[TYAutoClickTimerSaveData].toInt();
    } else {
        m_autoclickTimer = 0;
    }
}

int TYMainController::autoclickTimer()
{
    return m_autoclickTimer;
}

void TYMainController::setImage(QImage* aImage)
{
    if (m_mainView) {
        m_mainView->setImage(aImage);
        m_mainView->update();
    }
}

void TYMainController::setImage(QImage* aImage, const QRect& inRect)
{
    if (m_mainView) {
        m_mainView->setImage(aImage);
        if (m_isFullScreenMode) {
            m_mainView->setNeedsDisplayScalingRect(inRect);
        } else {
            m_mainView->update(inRect);
        }
    }
}

QImage* TYMainController::image()
{
    if (m_mainView) {
        return m_mainView->image();
    }
    return nullptr;
}

void TYMainController::directDrawImage(QImage* sourceImage, const QRect& inRect, const QRect& fromRect)
{
    if (m_mainView) {
        m_mainView->directDrawImage(sourceImage, inRect, fromRect);
    }
}

QImage* TYMainController::makeImageFromMainView()
{
    if (!m_mainView) {
        return nullptr;
    }

    QImage image(TYVirtualScreenSize(), QImage::Format_ARGB32);
    image.fill(Qt::black);

    // Render main view to image
    QPainter painter(&image);
    if (m_mainView->testAttribute(Qt::WA_PaintOnScreen)) {
        // For OpenGL-based widgets, would need GL ReadPixels
    }
    painter.end();

    return new QImage(image);
}

void TYMainController::setNeedsDisplayMainView(bool needs)
{
    if (m_mainView && needs) {
        m_mainView->update();
    }
}

void TYMainController::setAcceptsMouseMovedEvents(bool flag)
{
    Q_UNUSED(flag);
    // Note: In Qt, mouse tracking is controlled per-widget
}

QMainWindow* TYMainController::mainWindow()
{
    return m_mainWindow;
}

// TYController interface implementation
void TYMainController::select_action()
{
    if (m_trapSelectAction && m_tyStatus != TYPrinting) {
        if (m_scriptEngine) {
            m_scriptEngine->fireOfTrap();
        }
    }

    switch (m_tyStatus) {
    case TYClickWaitStatus:
        if (m_scriptEngine) {
            QTimer::singleShot(0, m_scriptEngine, SLOT(runScript()));
        }
        break;
    case TYPrinting:
        if (m_stageManager) {
            m_stageManager->select_action();
        }
        break;
    case TYButtonWaitStatus:
    case TYSelectStatus:
        if (m_stageManager) {
            m_stageManager->select_action();
        }
        break;
    default:
        break;
    }
}

void TYMainController::cancel_action(QObject* sender)
{
    Q_UNUSED(sender);
    switch (m_tyStatus) {
    case TYButtonWaitStatus:
        if (m_stageManager) {
            m_stageManager->cancel_action(sender);
        }
        break;
    case TYSelectStatus:
        openRightclickMenu(nullptr);
        break;
    case TYScriptRunningStatus:
    case TYClickWaitStatus:
        break;
    default:
        if (m_stageManager) {
            m_stageManager->cancel_action(sender);
        }
        break;
    }
}

void TYMainController::up_action()
{
    switch (m_tyStatus) {
    case TYButtonWaitStatus:
    case TYSelectStatus:
    case TYPrinting:
        if (m_stageManager) {
            m_stageManager->up_action();
        }
        break;
    default:
        break;
    }
}

void TYMainController::down_action()
{
    switch (m_tyStatus) {
    case TYButtonWaitStatus:
    case TYSelectStatus:
        if (m_stageManager) {
            m_stageManager->down_action();
        }
        break;
    default:
        break;
    }
}

void TYMainController::move_mouse(const QPoint& point)
{
    switch (m_tyStatus) {
    case TYButtonWaitStatus:
    case TYSelectStatus:
        if (m_stageManager) {
            m_stageManager->move_mouse(point);
        }
        break;
    default:
        break;
    }
}

void TYMainController::other_action(QObject* code)
{
    switch (m_tyStatus) {
    case TYButtonWaitStatus:
        if (m_stageManager) {
            m_stageManager->other_action(code);
        }
        break;
    default:
        break;
    }
}

// System mode methods
void TYMainController::enterSystemMode()
{
    m_isSystemMode = true;

    switch (m_tyStatus) {
    case TYPrinting:
        if (m_stageManager) {
            m_stageManager->enterSystemMode();
        }
        break;
    case TYScriptRunningStatus:
        if (m_scriptEngine) {
            m_scriptEngine->setBreakRun(true);
        }
        break;
    default:
        break;
    }

    if (m_mainView && m_stageManager) {
        m_mainView->setImage(m_stageManager->visualLayer());
    }
}

void TYMainController::exitSystemModeResumeStatus(bool aBool)
{
    switch (m_tyStatus & (TYRmodeStatusMask - 1)) {
    case TYPrinting:
    case TYSelectStatus:
        if (m_stageManager) {
            m_stageManager->exitSystemModeResumeStatus(aBool);
        }
        break;
    case TYScriptRunningStatus:
        if (m_scriptEngine) {
            QTimer::singleShot(100, m_scriptEngine, SLOT(runScript()));
        }
        break;
    default:
        break;
    }

    m_isSystemMode = false;
}

bool TYMainController::isPossibleEnterSystemMode()
{
    if (m_isSystemMode) {
        return false;
    }

    switch (m_tyStatus) {
    case TYPrinting:
        if (m_stageManager) {
            return m_stageManager->isPossibleEnterSystemMode();
        }
        return false;
    case TYSelectStatus:
        return true;
    default:
        return false;
    }
}

// IBActions implementation
void TYMainController::changeTextSpeed(QObject* sender)
{
    Q_UNUSED(sender);
    if (m_stageManager) {
        // Get speed from sender tag
    }
}

void TYMainController::skipToSelection(QObject* sender)
{
    Q_UNUSED(sender);
    if (m_stageManager) {
        m_stageManager->setSkipStatus(TYSelectionSkipMask);
    }
}

void TYMainController::skipWhileKidoku(QObject* sender)
{
    Q_UNUSED(sender);
    if (m_scriptEngine) {
        m_scriptEngine->setWatchKidoku(true);
    }
    if (m_stageManager) {
        m_stageManager->setSkipStatus(TYKidokuSkipMask);
    }
}

void TYMainController::versionDisp(QObject* sender)
{
    Q_UNUSED(sender);
    QMessageBox::information(nullptr, "Version", m_scriptVersionStr);
}

void TYMainController::openSaveDialog(QObject* sender)
{
    if (isPossibleEnterSystemMode()) {
        enterSaveLoadMenu(0);
    } else {
        QApplication::beep();
    }
}

void TYMainController::openLoadDialog(QObject* sender)
{
    if (isPossibleEnterSystemMode()) {
        enterSaveLoadMenu(1);
    } else {
        QApplication::beep();
    }
}

void TYMainController::enterSaveLoadMenu(int loadmode)
{
    if (!m_saveLoadController || !m_mainView) {
        return;
    }

    TYSaveLoadMenuView* aView = new TYSaveLoadMenuView(
        TYVirtualScreenRect(),
        this,
        m_rightMenuLayout,
        m_stageManager ? m_stageManager->textAttDict() : QMap<QString, QVariant>(),
        m_saveLoadController->dateStrings(loadmode),
        m_saveLoadController->existsArray(),
        loadmode
    );

    m_mainView->addChildWidget(aView);
    aView->show();
    aView->setFocus();
    aView->updatesTrackingRect();
    enterSystemMode();
}

void TYMainController::openAudioPathWindow(QObject* sender)
{
    Q_UNUSED(sender);
    if (!m_audioPathController) {
        m_audioPathController = new TYAudioPathController();
        // Note: In Qt, would load UI differently
    }

    m_audioPathController->openWithPath(m_audioSearchPath, "");
}

void TYMainController::showDebugInfo(QObject* sender)
{
    Q_UNUSED(sender);
    QString info = QString("Line: %1").arg(
        m_scriptEngine ? m_scriptEngine->runLine() + 1 : 0
    );
    QMessageBox::information(nullptr, "Debug Information", info);
}

void TYMainController::changeAudioPath(const QString& aPath)
{
    m_audioSearchPath = aPath;
    TYEnviroment::setObject(m_audioSearchPath, TYAudioFilePathEnviroment);

    if (m_audioPathController) {
        delete m_audioPathController;
        m_audioPathController = nullptr;
    }

    // Restart BGM if was playing
    if (!m_bgmPath.isEmpty()) {
        // Note: Need to check loop mode
        playSound(m_bgmPath, true, m_bgmForceSave);
    }
}

void TYMainController::windowMode(QObject* sender)
{
    Q_UNUSED(sender);
    if (!m_isFullScreenMode) {
        return;
    }

    if (m_isSystemMode) {
        QApplication::beep();
        return;
    }

    m_isFullScreenMode = false;

    // End full screen mode
    TYEndFullScreen();

    // Move main view back to main window
    if (m_mainView && m_mainWindow) {
        QRect mainFrame(0, 0, VSCREEN_WIDTH, VSCREEN_HEIGHT);
        m_mainView->setScaling(1.0f);
        m_mainView->setFrame(mainFrame);
    }

    if (m_mainWindow) {
        m_mainWindow->showNormal();
        m_mainWindow->activateWindow();
    }
}

void TYMainController::fullScreenMode(QObject* sender)
{
    Q_UNUSED(sender);
    if (m_isFullScreenMode) {
        return;
    }

    if (m_isSystemMode) {
        QApplication::beep();
        return;
    }

    // Enter full screen
    QSize fullScreenSize;
    if (TYEnviroment::boolForKey(TYDisplayResizeAtFullScreenEnviroment)) {
        if (TYEnviroment::boolForKey(TYHideOtherFullScreenEnviroment)) {
            QApplication::setOverrideCursor(Qt::BlankCursor);
        }
        fullScreenSize = TYStartFullScreen(VSCREEN_WIDTH, VSCREEN_HEIGHT);
    }

    if (m_fullScreenWindow) {
        m_fullScreenWindow->showFullScreen();
    }

    m_isFullScreenMode = true;
}

void TYMainController::openBGMVolumeDialog(QObject* sender)
{
    Q_UNUSED(sender);
    if (!m_volumeController) {
        float bV = 1.0f;
        QVariant temp = TYEnviroment::objectForKey(TYBGMVolumeEnviroment);
        if (temp.isValid()) {
            bV = temp.toFloat();
        }

        if (!m_mode_ext) {
            m_volumeController = new TYBGMVolumeController(bV);
        } else {
            float vV = 1.0f, sV = 1.0f;
            temp = TYEnviroment::objectForKey(TYVoiceVolumeEnviroment);
            if (temp.isValid()) vV = temp.toFloat();
            temp = TYEnviroment::objectForKey(TYSEVolumeEnviroment);
            if (temp.isValid()) sV = temp.toFloat();

            m_volumeController = new TYExtVolumeController(
                m_defBGMVol, bV, m_defVoiceVol, vV, m_defSEVol, sV
            );
        }
    }

    m_volumeController->setDelegate(this);
    m_volumeController->showDialog();
    closeBGMVolumeDialog(m_volumeController);
}

void TYMainController::preference(QObject* sender)
{
    Q_UNUSED(sender);
    TYPreferencePanelController::showDialog();
}

void TYMainController::changeBGMVolume(QObject* sender)
{
    Q_UNUSED(sender);
    // Note: Get volume from sender
    m_bgmVolume = 1.0f;
}

void TYMainController::closeBGMVolumeDialog(QObject* sender)
{
    Q_UNUSED(sender);
    TYEnviroment::setObject(m_bgmVolume, TYBGMVolumeEnviroment);

    if (m_mode_ext && m_volumeController) {
        TYEnviroment::setObject(m_volumeController->voiceVolume(), TYVoiceVolumeEnviroment);
        m_voiceVol = m_volumeController->voiceVolume() * 100 - 100;

        TYEnviroment::setObject(m_volumeController->seVolume(), TYSEVolumeEnviroment);
        m_seVol = m_volumeController->seVolume() * 100 - 100;
    }
}

void TYMainController::eraseTextWindowMode(QObject* sender)
{
    if (sender && !isPossibleEnterSystemMode()) {
        QApplication::beep();
        return;
    }

    if (!m_mainView) {
        return;
    }

    TYEraseModeView* aView = new TYEraseModeView(TYVirtualScreenRect(), this);
    m_mainView->addChildWidget(aView);
    aView->show();
    aView->setFocus();
    enterSystemMode();
}

void TYMainController::openRightclickMenu(QObject* sender)
{
    if (sender && !isPossibleEnterSystemMode()) {
        QApplication::beep();
        return;
    }

    if (!m_menuModeView) {
        m_menuModeView = new TYRootMenuView(
            TYVirtualScreenRect(),
            this,
            m_stageManager ? m_stageManager->textAttDict() : QMap<QString, QVariant>(),
            m_rightMenuItemArray,
            m_rightMenuLayout
        );
    }

    if (m_mainView) {
        m_mainView->addChildWidget(m_menuModeView);
        m_menuModeView->show();
        m_menuModeView->setFocus();
        m_menuModeView->updatesTrackingRect();
    }

    enterSystemMode();
}

void TYMainController::enterLookback(QObject* sender)
{
    if (sender && !isPossibleEnterSystemMode()) {
        QApplication::beep();
        return;
    }

    if (!m_stageManager) {
        return;
    }

    QVector<QImage*> queue = m_stageManager->lookbackQueue();
    if (queue.isEmpty()) {
        return;
    }

    // Create button images
    TYCellImage* img1 = nullptr;
    TYCellImage* img2 = nullptr;

    if (m_lookbackBtn.isEmpty()) {
        m_lookbackBtn = QStringList() << "uoncur.bmp" << "uoffcur.bmp"
                                       << "doncur.bmp" << "doffcur.bmp";
    }

    TYResourceServer* server = TYResourceServer::sharedServer();
    QImage* uoncur = server->getImage(m_lookbackBtn.value(1), true);
    QImage* uoffcur = server->getImage(m_lookbackBtn.value(0), true);
    QImage* doncur = server->getImage(m_lookbackBtn.value(3), true);
    QImage* doffcur = server->getImage(m_lookbackBtn.value(2), true);

    if (uoncur && uoffcur) {
        QVector<QImage*> imgs;
        imgs << uoncur << uoffcur;
        img1 = new TYCellImage(imgs);
    }

    if (doncur && doffcur) {
        QVector<QImage*> imgs;
        imgs << doncur << doffcur;
        img2 = new TYCellImage(imgs);
    }

    QMap<QString, QVariant> aDict = m_stageManager->textAttDict();
    if (m_lookbackColor.isValid()) {
        aDict[TYForegroundColorAttributeName] = m_lookbackColor;
    }

    if (!m_mainView) {
        return;
    }

    TYLookbackView* aView = new TYLookbackView(
        TYVirtualScreenRect(),
        this,
        aDict,
        m_rightMenuLayout,
        queue,
        img1,
        img2
    );

    m_mainView->addChildWidget(aView);
    aView->show();
    aView->setFocus();
    aView->updatesTrackingRect();

    enterSystemMode();
}

void TYMainController::resetGame(QObject* sender)
{
    if (sender) {
        // Show confirmation dialog
        if (QMessageBox::question(nullptr, "Reset Game", "Are you sure you want to reset?",
                                   QMessageBox::Ok | QMessageBox::Cancel) != QMessageBox::Ok) {
            return;
        }
    }

    if (status() == TYScriptRunningStatus && m_scriptEngine) {
        m_scriptEngine->setBreakRun(true);
    }

    setStatus(TYScriptRunningStatus);

    if (m_scriptEngine) {
        QTimer::singleShot(0, m_scriptEngine, SLOT(runScript()));
    }

    // Stop sounds
    m_bgmPath.clear();
    if (m_waveController) {
        delete m_waveController;
        m_waveController = nullptr;
    }

    m_autoclickTimer = 0;

    if (m_stageManager) {
        m_stageManager->resetGame();
    }
    if (m_scriptEngine) {
        m_scriptEngine->resetGame();
    }
}

void TYMainController::forceResume(QObject* sender)
{
    Q_UNUSED(sender);
    if (m_scriptEngine) {
        m_scriptEngine->runScript();
    }
}

void TYMainController::setEnableAutomode(bool aBool)
{
    m_enable_automode = aBool;
}

void TYMainController::openAutoMode(QObject* sender)
{
    Q_UNUSED(sender);
    static TYAutoModeController* autoDialog = nullptr;
    if (!autoDialog) {
        autoDialog = new TYAutoModeController();
    }

    if (!TYEnviroment::objectForKey(TYAutomodeWaitTimeEnviroment).isValid()) {
        TYEnviroment::setObject(0, TYAutomodeWaitTimeEnviroment);
    }

    int result = autoDialog->runModalTextSpeed(
        m_stageManager ? m_stageManager->userTextSpeed() : 0,
        TYEnviroment::objectForKey(TYAutomodeWaitTimeEnviroment).toInt(),
        TYEnviroment::objectForKey(TYAutomodeWaitTypeEnviroment).toInt()
    );

    if (result == QDialog::Accepted) {
        TYEnviroment::setObject(autoDialog->waitNum(), TYAutomodeWaitTimeEnviroment);
        TYEnviroment::setObject(autoDialog->waitType(), TYAutomodeWaitTypeEnviroment);

        if (m_stageManager) {
            m_stageManager->setAutoMode(
                autoDialog->textSpeed(),
                autoDialog->waitType(),
                autoDialog->waitNum()
            );
        }
    }
}

void TYMainController::enterYesNoModeTitle(const QString& title,
                                           const QString& yAction,
                                           const QString& nAction)
{
    if (!m_mainView) {
        return;
    }

    QStringList titleAndAction;
    titleAndAction << title << yAction << nAction;

    TYYesNoMenuView* aView = new TYYesNoMenuView(
        TYVirtualScreenRect(),
        this,
        m_stageManager ? m_stageManager->textAttDict() : QMap<QString, QVariant>(),
        titleAndAction,
        m_rightMenuLayout
    );

    m_mainView->addChildWidget(aView);
    aView->show();
    aView->setFocus();
    aView->updatesTrackingRect();
    enterSystemMode();
}

// Sound playback methods
void TYMainController::playSound(const QString& argment, bool loop, bool save)
{
    if (argment.isEmpty()) {
        ty_playstop(QVector<QVariant>());
        return;
    }

    m_bgmPath = argment;
    m_bgmForceSave = save;

    // Parse audio path with wildcard
    if (argment.startsWith("*")) {
        QString trackNo = argment.mid(1);
        QStringList components = m_audioSearchPath.split("*");
        if (components.size() >= 2) {
            QString path = QString("%1%2%3")
                               .arg(components[0])
                               .arg(trackNo.toInt(), 2, 10, QChar('0'))
                               .arg(components[1]);
            // Load movie from path
            Q_UNUSED(path);
        }
    } else {
        QString* filePath = TYResourceServer::sharedServer()->getFilePath(m_bgmPath);
        if (filePath) {
            // Load movie
            Q_UNUSED(filePath);
        } else {
            // Create temp file
            QString newTmpPath = getNScrRootDirectory() + "/." + winPathToUnix(m_bgmPath).split("/").last();
            if (m_tmpPath == newTmpPath) {
                // Already playing
                return;
            }

            QByteArray data = TYResourceServer::sharedServer()->getData(winPathToUnix(m_bgmPath));
            if (!data.isEmpty()) {
                m_tmpPath = newTmpPath;
                QFile file(m_tmpPath);
                if (file.open(QIODevice::WriteOnly)) {
                    file.write(data);
                    file.close();
                }
            } else {
                m_tmpPath.clear();
            }
        }
    }

    // Note: In Qt, would use QMediaPlayer or similar
    Q_UNUSED(loop);
    Q_UNUSED(save);
}

void TYMainController::playWaveSound(const QString& argment, bool loop)
{
    if (m_waveController) {
        m_waveController->stop();
        delete m_waveController;
    }

    m_waveController = new TYSoundController(argment, loop);

    if (m_enable_automode && !loop) {
        m_waveController->setPostingNotification(true);
    }

    m_waveController->play();
}

void TYMainController::playWaveChannel(int ch, const QString& aPath, bool loop)
{
    Q_ASSERT(ch >= 0 && ch < DIRECT_WAVE_MAX);

    int volume;
    switch (ch) {
    case 0:
    case DIRECT_WAVE_REPLAY:
        volume = (100 + m_voiceVol) * TYSOUND_VOLUME_MAX / 100;
        break;
    default:
        volume = (100 + m_seVol) * TYSOUND_VOLUME_MAX / 100;
    }

    if (m_directWaveController[ch]) {
        m_directWaveController[ch]->stop();
        delete m_directWaveController[ch];
    }

    m_directWaveController[ch] = new TYSoundController(aPath, loop, volume);

    if (ch == 0 && !loop) {
        m_directWaveController[ch]->setPostingNotification(true);
    }

    if (ch == 0) {
        emit voicePlayNotification(aPath);
    }

    m_usingDirectWaveCannelSet.insert(ch);
    m_directWaveController[ch]->play();
}

void TYMainController::playMovie(const QString& aPath, bool cancel)
{
    Q_UNUSED(cancel);
    if (!TYEnviroment::boolForKey(TYEnablePlayMovieEnviroment)) {
        return;
    }

    QString filePath = TYResourceServer::sharedServer()->getFilePathMakeTemp(aPath);
    if (filePath.isEmpty()) {
        return;
    }

    // Note: In Qt, would use QMediaPlayer or QVideoWidget
    Q_UNUSED(filePath);
}

void TYMainController::endCheckMovie()
{
    if (m_movieView) {
        // Check if still playing
        QTimer::singleShot(100, this, SLOT(endCheckMovie()));
    } else {
        endPlayMovie();
    }
}

void TYMainController::endPlayMovie()
{
    if (!m_movieView) {
        return;
    }

    if (m_movieView) {
        delete m_movieView;
        m_movieView = nullptr;
    }

    if (m_ctrlView) {
        delete m_ctrlView;
        m_ctrlView = nullptr;
    }

    if (m_isFullScreenMode && m_fullScreenWindow) {
        // Update content view
    }

    if (m_scriptEngine) {
        QTimer::singleShot(0, m_scriptEngine, SLOT(runScript()));
    }
}

// Script command methods
void TYMainController::ty_playstop(QVector<QVariant> argments)
{
    Q_UNUSED(argments);
    m_bgmPath.clear();
}

void TYMainController::ty_defvoicevol(QVector<QVariant> argments)
{
    if (!argments.isEmpty()) {
        m_defVoiceVol = argments[0].toInt() / 100.0f;
    }
}

void TYMainController::ty_defsevol(QVector<QVariant> argments)
{
    if (!argments.isEmpty()) {
        m_defSEVol = argments[0].toInt() / 100.0f;
    }
}

void TYMainController::ty_systemcall(QVector<QVariant> argments)
{
    if (argments.isEmpty()) {
        return;
    }

    QString arg = argments[0].toString().toLower();

    if (arg == "load") {
        enterSaveLoadMenu(1);
    } else if (arg == "save") {
        enterSaveLoadMenu(0);
    } else if (arg == "skip") {
        QVariant envObj = TYEnviroment::objectForKey(TYKidokumodeEnviroment);
        if (envObj.isValid() && envObj.toBool()) {
            skipWhileKidoku(nullptr);
        } else {
            skipToSelection(nullptr);
        }
    } else if (arg == "windowerase") {
        eraseTextWindowMode(nullptr);
    } else if (arg == "lookback") {
        enterLookback(nullptr);
    } else if (arg == "rmenu") {
        openRightclickMenu(nullptr);
    } else if (arg == "reset") {
        enterYesNoModeTitle("Reset?", "_reset", "");
    } else if (arg == "resetdlg") {
        resetGame(nullptr);
    }
}

void TYMainController::ty_menusetwindow(QVector<QVariant> argments)
{
    if (argments.size() < 7) {
        return;
    }

    QSize fontSize(argments[0].toInt(), argments[1].toInt());
    QSize pitchSize(argments[2].toInt(), argments[3].toInt());
    bool bold = argments[4].toBool();
    bool shadow = argments[5].toBool();
    QString colorStr = argments[6].toString();

    m_rightMenuLayout[TYRMenuFontSize] = fontSize;
    m_rightMenuLayout[TYRMenuFontPitchSize] = pitchSize;
    m_rightMenuLayout[TYRMenuShadowed] = shadow;
    m_rightMenuLayout[TYRMenuBold] = bold;
    m_rightMenuLayout[TYRMenuBGColor] = getTextWindowColorWithHTMLFormat(colorStr);
}

void TYMainController::ty_dwave(QVector<QVariant> argments)
{
    if (argments.size() < 2) {
        return;
    }

    playWaveChannel(argments[0].toInt(), argments[1].toString(), false);
}

void TYMainController::didChangeKeyWindow()
{
    if (m_isFullScreenMode) {
        // Handle window focus changes
    }
}

void TYMainController::lockFocusMainView()
{
    // In Qt, locking focus is handled differently
}

void TYMainController::unlockFocusMainView()
{
    // In Qt, unlocking focus is handled differently
}

int TYMainController::selectedSpeedTag()
{
    // Return current speed tag
    return -1;
}

void TYMainController::changeFont(QObject* fontManager)
{
    Q_UNUSED(fontManager);
    if (m_stageManager) {
        m_stageManager->changeFont(fontManager);
    }
}

// Script command methods
void TYMainController::ty_savenumber(QVector<QVariant> argments)
{
    if (argments.size() >= 2) {
        int num = argments[1].toInt();
        if (m_saveLoadController) {
            m_saveLoadController->setNumber(num);
        }
    }
}

void TYMainController::ty_savefileexist(QVector<QVariant> argments)
{
    if (argments.size() >= 3) {
        QString filename = QString(SAVEDATA_NAME).arg(argments[2].toInt());
        QString path = getNScrRootDirectory() + "/" + filename;
        bool exist = QFile::exists(path);
        QVector<QVariant> evalArg;
        evalArg << "mov" << argments[1] << (exist ? 1 : 0);
        if (m_scriptEngine) {
            m_scriptEngine->ty_mov(evalArg);
        }
    }
}

void TYMainController::ty_savetime(QVector<QVariant> argments)
{
    if (argments.size() < 2) return;

    QString filename = QString(SAVEDATA_NAME).arg(argments[1].toInt());
    QString path = getNScrRootDirectory() + "/" + filename;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        QVector<QVariant> evalArg;
        evalArg << "" << argments[2] << 0;
        if (m_scriptEngine) {
            m_scriptEngine->ty_mov(evalArg);
        }
    } else {
        QDataStream in(&file);
        QMap<QString, QVariant> aDict;
        in >> aDict;
        file.close();

        if (aDict.contains(TYMakeDateSaveData)) {
            QDateTime date = aDict[TYMakeDateSaveData].toDateTime();
            int dateValue[4];
            dateValue[0] = date.date().month();
            dateValue[1] = date.date().day();
            dateValue[2] = date.time().hour();
            dateValue[3] = date.time().minute();

            for (int i = 0; i < 4; i++) {
                QVector<QVariant> evalArg;
                evalArg << "" << argments[2 + i] << dateValue[i];
                if (m_scriptEngine) {
                    m_scriptEngine->ty_mov(evalArg);
                }
            }
        }
    }
}

void TYMainController::ty_filelog(QVector<QVariant> argments)
{
    Q_UNUSED(argments);
    TYResourceServer::sharedServer()->filelog(
        getNScrRootDirectory() + "/" + FILELOG_FILENAME
    );
}

void TYMainController::ty_mp3(QVector<QVariant> argments)
{
    if (argments.size() >= 2) {
        QString arg = argments[1].toString();
        playSound(arg, false, false);
    }
}

void TYMainController::ty_mp3save(QVector<QVariant> argments)
{
    if (argments.size() >= 2) {
        QString arg = argments[1].toString();
        playSound(arg, false, true);
    }
}

void TYMainController::ty_mp3loop(QVector<QVariant> argments)
{
    if (argments.size() >= 2) {
        QString arg = argments[1].toString();
        playSound(arg, true, false);
    }
}

void TYMainController::ty_mp3stop(QVector<QVariant> argments)
{
    ty_playstop(argments);
}

void TYMainController::ty_play(QVector<QVariant> argments)
{
    if (argments.size() >= 2) {
        QString arg = argments[1].toString();
        playSound(arg, true, false);
    }
}

void TYMainController::ty_playonce(QVector<QVariant> argments)
{
    if (argments.size() >= 2) {
        QString arg = argments[1].toString();
        playSound(arg, false, false);
    }
}

void TYMainController::ty_bgm(QVector<QVariant> argments)
{
    if (argments.size() >= 2) {
        QString arg = argments[1].toString();
        playSound(arg, true, false);
    }
}

void TYMainController::ty_bgmonce(QVector<QVariant> argments)
{
    if (argments.size() >= 2) {
        QString arg = argments[1].toString();
        playSound(arg, false, false);
    }
}

void TYMainController::idleTimer()
{
    // Qt implementation - using QMediaPlayer or similar
    // This was used for QuickTime movie playback
}

void TYMainController::ty_avi(QVector<QVariant> argments)
{
    if (argments.size() >= 3) {
        QString path = argments[1].toString();
        bool cancel = argments[2].toBool();
        playMovie(path, cancel);
    }
}

void TYMainController::ty_mpegplay(QVector<QVariant> argments)
{
    if (argments.size() >= 3) {
        QString path = argments[1].toString();
        bool cancel = argments[2].toBool();
        playMovie(path, cancel);
    }
}

void TYMainController::ty_v(QVector<QVariant> argments)
{
    if (argments.size() >= 2) {
        QString path = QString(INSTANT_WAVE_PATH_FORMAT).arg(argments[1].toString());
        playWaveSound(path, false);
    }
}

void TYMainController::ty_wave(QVector<QVariant> argments)
{
    if (argments.size() >= 2) {
        QString arg = argments[1].toString();
        playWaveSound(arg, false);
    }
}

void TYMainController::ty_waveloop(QVector<QVariant> argments)
{
    if (argments.size() >= 2) {
        QString arg = argments[1].toString();
        playWaveSound(arg, true);
    }
}

void TYMainController::ty_wavestop(QVector<QVariant> argments)
{
    Q_UNUSED(argments);
    if (m_waveController) {
        m_waveController->stop();
    }
}

void TYMainController::ty_dv(QVector<QVariant> argments)
{
    if (argments.size() >= 2) {
        QString path = QString(INSTANT_WAVE_PATH_FORMAT).arg(argments[1].toString());
        playWaveChannel(0, path, false);
    }
}

void TYMainController::ty_dwaveloop(QVector<QVariant> argments)
{
    if (argments.size() >= 3) {
        int ch = argments[1].toInt();
        QString path = argments[2].toString();
        playWaveChannel(ch, path, true);
    }
}

void TYMainController::ty_dwavestop(QVector<QVariant> argments)
{
    if (argments.size() >= 2) {
        int ch = argments[1].toInt();
        Q_ASSERT(ch >= 0 && ch < DIRECT_WAVE_MAX);
        if (m_directWaveController[ch]) {
            m_directWaveController[ch]->stop();
        }
    }
}

void TYMainController::ty_stop(QVector<QVariant> argments)
{
    Q_UNUSED(argments);

    // Stop BGM
    m_bgmPath.clear();

    // Stop wave controller
    if (m_waveController) {
        m_waveController->stop();
    }

    // Stop all direct wave controllers
    QSet<int>::iterator it;
    for (it = m_usingDirectWaveCannelSet.begin(); it != m_usingDirectWaveCannelSet.end(); ++it) {
        int ch = *it;
        if (m_directWaveController[ch]) {
            m_directWaveController[ch]->stop();
        }
    }
}

void TYMainController::ty_caption(QVector<QVariant> argments)
{
    if (argments.size() >= 2 && m_mainWindow) {
        m_mainWindow->setWindowTitle(argments[1].toString());
    }
}

void TYMainController::ty_versionstr(QVector<QVariant> argments)
{
    if (argments.size() >= 3) {
        m_scriptVersionStr = argments[1].toString() + "\n" + argments[2].toString();
    }
}

void TYMainController::ty_click(QVector<QVariant> argments)
{
    Q_UNUSED(argments);
    if (m_scriptEngine) {
        m_scriptEngine->setBreakRun(true);
    }
    setStatus(TYClickWaitStatus);
}

void TYMainController::ty_autoclick(QVector<QVariant> argments)
{
    if (argments.size() >= 2) {
        m_autoclickTimer = argments[1].toInt();
    }
}

void TYMainController::ty_wait(QVector<QVariant> argments)
{
    if (m_scriptEngine) {
        m_scriptEngine->setBreakRun(true);
    }
    setStatus(TYTimeWaitStatus);
    if (argments.size() >= 2) {
        int delay = argments[1].toInt() / 1000;
        QTimer::singleShot(delay, m_scriptEngine, SLOT(runScript()));
    }
}

void TYMainController::ty_delay(QVector<QVariant> argments)
{
    if (m_scriptEngine) {
        m_scriptEngine->setBreakRun(true);
    }
    setStatus(TYClickWaitStatus);
    if (argments.size() >= 2) {
        int delay = argments[1].toInt() / 1000;
        QTimer::singleShot(delay, this, SLOT(select_action()));
    }
}

void TYMainController::ty_spi(QVector<QVariant> argments)
{
    if (argments.size() >= 2) {
        QString arg = argments[1].toString();
        QStringList argArray = arg.split("|");
        if (argArray.size() >= 2) {
            TYResourceServer::sharedServer()->addSpi(argArray[0], argArray[1]);
        }
    }
}

void TYMainController::ty_soundpressplugin(QVector<QVariant> argments)
{
    if (argments.size() >= 2) {
        QString arg = argments[1].toString();
        QStringList argArray = arg.split("|");
        if (argArray.size() >= 2) {
            TYResourceServer::sharedServer()->addSoundPressPlugin(argArray[0], argArray[1]);
        }
    }
}

void TYMainController::ty_mode_ext(QVector<QVariant> argments)
{
    Q_UNUSED(argments);
    m_mode_ext = true;
    setEnableAutomode(true);
}

void TYMainController::ty_automode_time(QVector<QVariant> argments)
{
    if (!m_enable_automode) {
        setEnableAutomode(true);
    }

    if (!TYEnviroment::objectForKey(TYAutomodeWaitTimeEnviroment).isValid()) {
        if (argments.size() >= 2) {
            int temp = argments[1].toInt();
            TYEnviroment::setObject(temp, TYAutomodeWaitTimeEnviroment);
        }
    }
}

void TYMainController::ty_voicevol(QVector<QVariant> argments)
{
    if (argments.size() >= 2) {
        m_voiceVol = argments[1].toInt() - 100;
    }
}

void TYMainController::ty_mp3vol(QVector<QVariant> argments)
{
    if (argments.size() >= 2) {
        int volume = argments[1].toInt();
        m_bgmVolume = volume / 100.0f;
        // Note: Update BGM view volume if available
    }
}

void TYMainController::ty_kidokuskip(QVector<QVariant> argments)
{
    Q_UNUSED(argments);
    if (!TYEnviroment::objectForKey(TYKidokumodeEnviroment).isValid()) {
        TYEnviroment::setObject(true, TYKidokumodeEnviroment);
    }
    if (m_skipMenu) {
        // Update skip menu title
    }
    if (m_scriptEngine) {
        m_scriptEngine->enableKidoku();
    }
}

void TYMainController::ty_kidokumode(QVector<QVariant> argments)
{
    if (argments.size() >= 2) {
        bool val = argments[1].toInt() != 0;
        TYEnviroment::setObject(val, TYKidokumodeEnviroment);
    }
}

void TYMainController::ty_reset(QVector<QVariant> argments)
{
    Q_UNUSED(argments);
    resetGame(nullptr);
}

void TYMainController::ty_menu_window(QVector<QVariant> argments)
{
    Q_UNUSED(argments);
    windowMode(nullptr);
}

void TYMainController::ty_menu_full(QVector<QVariant> argments)
{
    Q_UNUSED(argments);
    fullScreenMode(nullptr);
}

void TYMainController::ty_savename(QVector<QVariant> argments)
{
    if (argments.size() >= 4 && m_saveLoadController) {
        QStringList names;
        names << argments[1].toString() << argments[2].toString() << argments[3].toString();
        m_saveLoadController->setNames(names);
    }
}

void TYMainController::ty_rmenu(QVector<QVariant> argments)
{
    m_rightMenuItemArray.clear();

    for (int i = 1; i < argments.size(); i += 2) {
        if (m_scriptEngine) {
            m_rightMenuItemArray.append(m_scriptEngine->getStringOfArgment(argments[i].toString()));
        }
        if (i + 1 < argments.size()) {
            m_rightMenuItemArray.append(argments[i + 1].toString());
        }
    }
}

void TYMainController::ty_menusetwindow(QVector<QVariant> argments)
{
    if (argments.size() < 8) return;

    QSize fontSize(argments[1].toInt(), argments[2].toInt());
    QSize pitchSize(argments[3].toInt(), argments[4].toInt());
    bool bold = argments[5].toBool();
    bool shadow = argments[6].toBool();
    QString colorStr = argments[7].toString();

    m_rightMenuLayout[TYRMenuFontSize] = QVariant::fromValue(fontSize);
    m_rightMenuLayout[TYRMenuFontPitchSize] = QVariant::fromValue(pitchSize);
    m_rightMenuLayout[TYRMenuShadowed] = shadow;
    m_rightMenuLayout[TYRMenuBold] = bold;
    m_rightMenuLayout[TYRMenuBGColor] = getTextWindowColorWithHTMLFormat(colorStr);
}

void TYMainController::ty_menuselectcolor(QVector<QVariant> argments)
{
    if (argments.size() < 4) return;

    if (!m_rightMenuLayout.isEmpty()) {
        m_rightMenuLayout[TYRMenuSelectColor] = getColorWithHTMLFormat(argments[1].toString());
        m_rightMenuLayout[TYRMenuNoSelectColor] = getColorWithHTMLFormat(argments[2].toString());
        m_rightMenuLayout[TYRMenuNoDataColor] = getColorWithHTMLFormat(argments[3].toString());
    }
}

void TYMainController::ty_lookbackcolor(QVector<QVariant> argments)
{
    if (argments.size() >= 2) {
        m_lookbackColor = getColorWithHTMLFormat(argments[1].toString());
    }
}

void TYMainController::ty_lookbackbutton(QVector<QVariant> argments)
{
    if (argments.size() >= 5) {
        m_lookbackBtn.clear();
        m_lookbackBtn << argments[1].toString()
                      << argments[2].toString()
                      << argments[3].toString()
                      << argments[4].toString();
    }
}

void TYMainController::ty_getmousepos(QVector<QVariant> argments)
{
    if (argments.size() < 3) return;

    QPoint cursorPos = QCursor::pos();
    QPoint windowPos;
    QPoint viewPos;

    if (m_mainView) {
        if (m_isFullScreenMode && m_fullScreenWindow) {
            windowPos = m_fullScreenWindow->pos();
            viewPos = m_mainView->pos();
        } else if (m_mainWindow) {
            windowPos = m_mainWindow->pos();
            viewPos = m_mainView->pos();
        }

        QPoint btnClickPoint(
            cursorPos.x() - windowPos.x() - viewPos.x(),
            cursorPos.y() - windowPos.y() - viewPos.y()
        );

        // Clamp to screen bounds
        if (btnClickPoint.x() < 0) btnClickPoint.setX(0);
        else if (btnClickPoint.x() > VSCREEN_WIDTH) btnClickPoint.setX(VSCREEN_WIDTH);

        if (btnClickPoint.y() < 0) btnClickPoint.setY(0);
        else if (btnClickPoint.y() > VSCREEN_HEIGHT) btnClickPoint.setY(VSCREEN_HEIGHT);

        btnClickPoint.setY(VSCREEN_HEIGHT - btnClickPoint.y());

        if (m_scriptEngine) {
            m_scriptEngine->ty_mov(QVector<QVariant>() << "mov" << argments[1] << btnClickPoint.x());
            m_scriptEngine->ty_mov(QVector<QVariant>() << "mov" << argments[2] << btnClickPoint.y());
        }
    }
}

void TYMainController::ty_exec_dll(QVector<QVariant> argments)
{
    if (argments.size() >= 2) {
        QString arg = argments[1].toString();
        TYResourceServer::sharedServer()->executeBundle(arg);
        if (m_scriptEngine) {
            m_scriptEngine->setBreakRun(true);
        }
    }
}

void TYMainController::ty_getret(QVector<QVariant> argments)
{
    if (argments.size() < 2) return;

    TYArgment* arg = nullptr; // Need to get argment from script engine
    if (arg) {
        if (arg->varType() & TYDigitVarTypeMask) {
            if (m_scriptEngine) {
                m_scriptEngine->ty_mov(QVector<QVariant>() << "mov" << QVariant::fromValue(arg) << TYCCSProxy::proxy()->returnCode());
            }
        } else if (arg->varType() == TYStringVarType) {
            if (m_scriptEngine) {
                m_scriptEngine->ty_mov(QVector<QVariant>() << "mov" << QVariant::fromValue(arg) << TYCCSProxy::proxy()->returnString());
            }
        }
    }
}

void TYMainController::setTrap(bool aBool)
{
    m_trapSelectAction = aBool;
}

void TYMainController::didChangeKeyWindow()
{
    if (m_isFullScreenMode) {
        // Handle window focus changes for full screen mode
    }
}
