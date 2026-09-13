//
//  TYMainController.h
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

#ifndef TYMAINCONTROLLER_H
#define TYMAINCONTROLLER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QRect>
#include <QPoint>
#include <QSize>
#include <QImage>
#include <QColor>
#include <QWidget>
#include <QMainWindow>
#include <QTimer>
#include <QSet>
#include <QMap>
#include <QVariant>

#include "TYSaveDataCoding.h"

// Forward declarations
class TYScriptEngine;
class TYStageManager;
class TYSoundController;
class TYSaveLoadManager;
class TYExtVolumeController;
class TYVisualNovelView;
class TYAudioPathController;
class TYMenuModeView;
class TYMovieControllerView;

// Status enum
enum {
    TYScriptRunningStatus = 1,
    TYClickWaitStatus = 2,
    TYTimeWaitStatus = 3,
    TYSelectStatus = 4,
    TYButtonWaitStatus = 5,
    TYPrinting = 6,
    TYRmodeStatusMask = 8
};

// Constants
#define SAVEDATA_NAME "save%d.msav"
#define REGISTRY_NAME "REGISTRY.plist"
#define AUDIO_TEMP ".temp"
#define DIRECT_WAVE_MAX 51
#define DIRECT_WAVE_REPLAY 50

// External constants
extern const QString TYRMenuFontSize;
extern const QString TYRMenuFontPitchSize;
extern const QString TYRMenuShadowed;
extern const QString TYRMenuBold;
extern const QString TYRMenuBGColor;
extern const QString TYRMenuSelectColor;
extern const QString TYRMenuNoSelectColor;
extern const QString TYRMenuNoDataColor;

extern const QString TYVoicePlayNotification;

// Controller interface
class TYController {
public:
    virtual ~TYController() {}
    virtual void select_action() = 0;
    virtual void cancel_action(QObject* sender) = 0;
    virtual void up_action() = 0;
    virtual void down_action() = 0;
    virtual void move_mouse(const QPoint& point) = 0;
    virtual void other_action(QObject* code) = 0;
};

// Main controller class
class TYMainController : public QObject, public TYController, public TYSaveDataCoding
{
    Q_OBJECT
    Q_PROPERTY(QObject* mainMenu READ mainMenu)
    Q_PROPERTY(TYVisualNovelView* mainView READ mainView)
    Q_PROPERTY(QMainWindow* mainWindow READ mainWindow)

public:
    explicit TYMainController(QObject* parent = nullptr);
    ~TYMainController();

    // Singleton
    static TYMainController* controller();

    // Application delegate methods
    void applicationDidFinishLaunching();
    int applicationShouldTerminate();

    // IBActions (converted to public slots)
public slots:
    void changeTextSpeed(QObject* sender);
    void skipToSelection(QObject* sender);
    void skipWhileKidoku(QObject* sender);
    void openAutoMode(QObject* sender);
    void versionDisp(QObject* sender);
    void openSaveDialog(QObject* sender);
    void openLoadDialog(QObject* sender);
    void openAudioPathWindow(QObject* sender);
    void showDebugInfo(QObject* sender);
    void windowMode(QObject* sender);
    void fullScreenMode(QObject* sender);
    void openBGMVolumeDialog(QObject* sender);
    void eraseTextWindowMode(QObject* sender);
    void preference(QObject* sender);
    void resetGame(QObject* sender);
    void forceResume(QObject* sender);
    void openRightclickMenu(QObject* sender);
    void enterLookback(QObject* sender);

    // Public methods
    void enterSystemMode();
    void exitSystemModeResumeStatus(bool aBool);
    void enterYesNoModeTitle(const QString& title, const QString& yAction, const QString& nAction);
    bool isPossibleEnterSystemMode();

    static void alertAndTerminate(const QString& title, const QString& msg);

    void enterSaveLoadMenu(int loadmode);
    void closeBGMVolumeDialog(QObject* sender);
    void changeBGMVolume(QObject* sender);

    void setStatus(int aStatus);
    int status();
    void saveLocalData(int number);
    bool loadLocalData(int number);

    int autoclickTimer();
    void setTrap(bool aBool);
//-(void)endButton;

    void setImage(QImage* aImage);
    void setImage(QImage* aImage, const QRect& inRect);
    QImage* image();
    void directDrawImage(QImage* sourceImage, const QRect& inRect, const QRect& fromRect);
    QImage* makeImageFromMainView();
    void setNeedsDisplayMainView(bool needs);
    void setAcceptsMouseMovedEvents(bool flag);

    void disableContextMenu();

    void playSound(const QString& argment, bool loop, bool save); // private
    void playWaveSound(const QString& argment, bool loop); // private
    void playWaveChannel(int ch, const QString& aPath, bool loop); // private
    void playMovie(const QString& aPath, bool cancel);
    void endCheckMovie();
    void endPlayMovie();

    void lockFocusMainView();
    void unlockFocusMainView();
    QMainWindow* mainWindow();
    QWidget* targetWindow() const { return m_targetWindow; }

    void changeFont(QObject* fontManager);

    void setAudioPath(const QString& aPath);

    int selectedSpeedTag();
// recept Notification
    void didChangeKeyWindow();

    void setEnableAutomode(bool aBool);

    // TYController interface
    void select_action() override;
    void cancel_action(QObject* sender) override;
    void up_action() override;
    void down_action() override;
    void move_mouse(const QPoint& point) override;
    void other_action(QObject* code) override;

// 公開ハンドラ
    void ty_playstop(QVector<QVariant> argments);
    void ty_defvoicevol(QVector<QVariant> argments);
    void ty_defsevol(QVector<QVariant> argments);
    void ty_systemcall(QVector<QVariant> argments);
    void ty_menusetwindow(QVector<QVariant> argments);
    void ty_dwave(QVector<QVariant> argments);

    // Accessors
    QObject* mainMenu() const { return m_mainMenu; }
    TYVisualNovelView* mainView() const { return m_mainView; }

signals:
    void voicePlayNotification(const QString& path);

private:
    // Private members
    QObject* m_mainMenu;
    TYVisualNovelView* m_mainView;
    QWidget* m_targetWindow;
    QWidget* m_defaultContentView;
    QMainWindow* m_mainWindow;
    QWidget* m_fullScreenWindow;
    QObject* m_speedMenu;
    QObject* m_skipMenu;
    QObject* m_nscrMenu;
    QObject* m_contextMenu;

    TYSaveLoadManager* m_saveLoadController;

    QWidget* m_bgmView;
    void* m_qtMovie;
    QTimer* m_gMyTimer;
    QString m_bgmPath;
    bool m_bgmForceSave;
    QString m_tmpPath;

    bool m_isFullScreenMode;

    TYScriptEngine* m_scriptEngine;
    TYStageManager* m_stageManager;

    QString m_audioSearchPath;
    int m_autoclickTimer;

    int m_tyStatus;

    QString m_scriptVersionStr;

    bool m_trapSelectAction;

    TYAudioPathController* m_audioPathController;

    float m_bgmVolume;
    TYExtVolumeController* m_volumeController;
    float m_defBGMVol;
    float m_defVoiceVol;
    float m_defSEVol;
    TYSoundController* m_waveController;
    TYSoundController* m_directWaveController[DIRECT_WAVE_MAX];
    int m_voiceVol;
    int m_seVol;
    QSet<int> m_usingDirectWaveCannelSet;

    bool m_mode_ext;
    bool m_enable_automode;
    int m_automode_time;

    bool m_isModal;

    QWidget* m_movieView;
    TYMovieControllerView* m_ctrlView;

    // Right-click menu
    QVector<QString> m_rightMenuItemArray;
    QMap<QString, QVariant> m_rightMenuLayout;
    TYMenuModeView* m_menuModeView;
    bool m_isSystemMode;
    QColor m_lookbackColor;
    QStringList m_lookbackBtn;
};

#endif // TYMAINCONTROLLER_H
