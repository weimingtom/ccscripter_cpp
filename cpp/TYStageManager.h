//
//  TYStageManager.h
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

#ifndef TYSTAGEMANAGER_H
#define TYSTAGEMANAGER_H

#include <QObject>
#include <QImage>
#include <QRect>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QList>
#include <QMap>
#include <QColor>
#include <QFont>
#include <QPainter>
#include <QTimer>
#include <QDateTime>
#include <QByteArray>
#include <QSet>
#include <QChar>

class TYMainController;
class TYScriptEngine;
class TYEffectGenerater;
class TYEffecter;
class TYStandingChar;
class TYWaitCursor;
class TYExtraButtonManager;
class TYSelectionManager;
class TYNovelLayer;
class TYNScrSprite;
class TYCellImage;
class TYStringImage;
class TYResourceServer;
class TYEffectDefinitionValue;
class TYBar;
class QLayoutManager;

enum TYSkipStatusMask {
    TYNoSkip = 0,
    TYOnetimeSkipMask = 1,
    TYSelectionSkipMask = 1 << 1,
    TYKidokuSkipMask = 1 << 2,
    TYAutoModeSkipMask = 1 << 3,
    TYEffectCancelSkipMask = TYSelectionSkipMask | TYKidokuSkipMask ,
    TYNoTimeWaitSkipMask = TYOnetimeSkipMask | TYSelectionSkipMask | TYKidokuSkipMask,
};

#define BAR_NUMBER_MAX 99

// オフスクリーンバッファの変更を行うクラス。テキスト表示中の制御も兼ねてる
class TYStageManager : public QObject, public TYController
{
    Q_OBJECT
private:
    TYMainController* m_controller; // イメージを引き渡すコントローラー。
    TYScriptEngine* m_engine; // スクリプトエンジン

    QImage m_visualLayer;
    QImage m_compositeLayer; // 画像レイヤーとテキストレイヤーを統合したレイヤー
    QMap<int, QVariant> m_effectDefDict; // エフェクト定義登録辞書
    TYEffectGenerater* m_effectGenerater; // エフェクト効果作成クラス
    QObject* m_afterEffectTarget; // エフェクト終了時に叩かれるオブジェクト
    QString m_afterEffectSelector; // 同セレクタ
    QVariant m_afterEffectObject; // 同引数オブジェクト
    //TYEffecter *effecter; // エフェクトを実現する下請けオブジェクト。
    
    // 背景情報
    QImage m_bgImage;
    QString m_bgPath;
    QRect m_bgRect;

    // 立ちキャラ情報
    float m_underline;
    TYStandingChar* m_leftCharImage;
    TYStandingChar* m_centerCharImage;
    TYStandingChar* m_rightCharImage;

    // スプライト
    QList<QObject*> m_spriteArray;
    int m_humanz;
    bool m_isUseHumanz;
    bool m_isWindowback;
    QList<QVariant> m_hideSpriteArray;

    // 特殊フィルタ
    QByteArray m_monocroSample;
    int m_nega;
    
    //NSMutableArray *textQueue;
    QImage m_novelLayer;
    QString m_textQueue;

    QImage m_overNovelLayer;

    QMap<QString, QVariant> m_textAttDict;
    QMap<QString, QVariant> m_textShadowAttDict;
    QString m_textFontName;
    QFont m_textFont;
    QSize m_displayFontSize;
    QColor m_textFontColor;

    bool m_novelLayerOn;
    bool m_eraseTextWindow;

    int m_textLeftOffset;
    int m_textTopOffset;
    int m_textColumn;
    int m_textRow;
    int m_textFontWidth;
    int m_textFontHeight;
    int m_textPitchx;
    int m_textPitchy;
    int* m_textSpeed;
    int m_userTextSpeed;
    int m_scriptTextSpeed;
    QList<QVariant> m_defaultSpeed;
    int m_selectedSpeedTag;
    bool m_textBold;
    bool m_textShadowed;

    int m_locateX;

    QObject* m_textWindow;
    QString m_textWindowPath;
    QRect m_textWindowRect;
    //NSPoint textWindowPoint;
    //NSSize textWindowSize;
    
    enum TYTPStatus { // テキスト表示状態時のステータス
        TYTPOff,
        TYTPPrinting,
        TYTPClickWait,
        TYTPTimeWait,
    } m_tpStatus;
    bool m_waitNewPage; // 改頁待ちフラグ
    bool m_forceClickWait; // 強制クリック待ちフラグ
    bool m_forceWaitNewPage; // 強制改ページ待ちフラグ
    bool m_invalidNewLine; // 次改行無効フラグ
    QSet<QChar> m_clickStrSet;
    int m_clickStrUnderLimit;
    QSet<QChar> m_hyphenationSet; // 禁則処理用文字セット

    int m_nowColumn;
    int m_nowRow;
    int m_nowStorageLength;
    int m_nowStorageOffset;
    
    //NSString *textQueue;

    // エフェクト終了後に起動するメソッドとレシーバー
    /*
    id nextTarget;
    id nextObject;
    SEL nextSelector;
    */
    
    // ボタン処理関連
    TYExtraButtonManager* m_buttonManager;
    int m_beforeSelectButtonID;
    QString m_buttonTargetValue;
    bool m_releaseButtonAfterSelect;
    bool m_isDrawButton;
    QVariant m_btnTime;
    QDateTime m_btnStartDate;
    
    // 選択肢処理
    TYSelectionManager* m_selectionManager;
    int m_beforeSelectIndex;
    QList<QList<QVariant>> m_selectAction;
    QColor m_selectColor;
    QColor m_unSelectColor;
    
    // スキップ制御
    int m_skipStatus;
    int m_automodeWait;
    int m_automodeWaitType;
    int m_waitCountOfChar;
    bool m_playingSoundFlg;
    bool m_waitFinishSound;
    bool m_waitFinishSoundOnBtnTime2;

    // クリック待ちカーソル
    TYWaitCursor* m_clickWaitCursor;
    TYWaitCursor* m_pageWaitCursor;
    TYWaitCursor* m_currentCursor;

    bool m_linepage,m_linepageWait;
    
    // システムカスタマイズ
    QString m_textgosubLabel;
    bool m_isPage;
    QList<QVariant> m_cselInfo; // 文字列、コマンドの順に格納。
    QList<QObject*> m_cselImage;

    QList<QVariant> m_lookbackQueue;

    QMap<QString, QObject*> m_barDict;

    TYNovelLayer* m_novelLayer;

    QTimer* m_printTimer;
    QTimer* m_autoTimer;

    static TYStageManager* s_sharedManager;
    static QSet<QChar> s_characterOfReferenceVarSet;
public:
    static TYStageManager* sharedManager();
    static void setModeSVGA();

    explicit TYStageManager(QObject* parent = nullptr);
    ~TYStageManager();

    void setController(TYMainController* aController, TYScriptEngine* aEngine);

    // System mode
    void enterSystemMode();
    void exitSystemModeResumeStatus(bool aBool);
    bool isPossibleEnterSystemMode();

    // Text window
    void loadTextWindow(const QString& path);
    void loadBGImage(const QString& aPath);
    QRect textWindowRect() const { return m_textWindowRect; }
    void drawTextWindow();
    QPoint drawPoint();

    // Sprite management
    void addSprite(QList<QVariant> argments, bool aBool);
    void setSprite(int idNo, bool aBool);
    void setSprite(int idNo, int cellNo);
    QRect rectOfSprite(int idNo);

    // Selection
    void startSelect(const QList<QString>& messages, const QList<QList<QVariant>>& respondArray);

    // Stage update
    void updateStage();
    void updateStageOnlyButton();
    QImage visualLayer() const { return m_visualLayer; }
    QImage compositeLayer() const { return m_compositeLayer; }
    void changeVisualLayer(int effectNum);
    void flushNovelLayer();
    void startEraseTextWindow(int effectNum);

    // Lookback
    QList<QVariant> lookbackQueue() const { return m_lookbackQueue; }

    // Text operations
    void textPrint(const QString& aString);
    void drawChar(const QString& charStr, const QColor& color, int column, int row);
    void drawAttStr(const QString& str, int column, int row);

    // Skip and status
    void setSkipStatus(int aStatus);
    int skipStatus() const { return m_skipStatus; }
    void setClickWait(bool pageWait);
    int status() const { return m_tpStatus; }
    void setStatus(int status);

    // Text attributes
    void makeAttributeDict();
    QMap<QString, QVariant> textAttDict() const { return m_textAttDict; }
    int textFontWidth() const { return m_textFontWidth; }
    int textFontHeight() const { return m_textFontHeight; }
    int textPitchx() const { return m_textPitchx; }
    bool isShadow() const { return m_textShadowed; }

    // Font and speed
    void changeFont(QFont* fontManager);
    void changeTextSpeed(int index);
    void setAutoMode(int speed, int waitType, int wait);
    int userTextSpeed() const { return m_userTextSpeed; }

    // Button operations
    void startButtonWait(QList<QVariant> argments, bool releaseAfterSelect);
    void changeSelectedButton(unsigned int btnIndex);
    void responseButton(int result);
    void changeSelection(int index);
    void responseSelection();
    void makeCselImage();

    // Effect callbacks
    void changeEffectionImage(const QImage& drawImage);
    void effectFinished();
    void quakeFinished();

    // Notification handlers
    void updateSprites();
    void startWaveSound();
    void stopWaveSound();

    // Save/Load
    QMap<QString, QVariant> encodeWithSaveData();
    void decodeWithSaveData(const QMap<QString, QVariant>& aObject);

    // Script commands
    void ty_effect(QList<QVariant> argments);
    void ty_windoweffect(QList<QVariant> argments);
    void ty_setwindow(QList<QVariant> argments);
    void ty_erasetextwindow(QList<QVariant> argments);
    void ty_textoff(QList<QVariant> argments);
    void ty_texton(QList<QVariant> argments);
    void ty_bg(QList<QVariant> argments);
    void ty_transmode(QList<QVariant> argments);
    void ty_underline(QList<QVariant> argments);
    void ty_ld(QList<QVariant> argments);
    void ty_cl(QList<QVariant> argments);
    void ty_humanz(QList<QVariant> argments);
    void ty_lsp(QList<QVariant> argments);
    void ty_lsph(QList<QVariant> argments);
    void ty_csp(QList<QVariant> argments);
    void ty_vsp(QList<QVariant> argments);
    void ty_msp(QList<QVariant> argments);
    void ty_amsp(QList<QVariant> argments);
    void ty_cell(QList<QVariant> argments);
    void ty_allsphide(QList<QVariant> argments);
    void ty_allspresume(QList<QVariant> argments);
    void ty_spstr(QList<QVariant> argments);
    void ty_bar(QList<QVariant> argments);
    void ty_barclear(QList<QVariant> argments);
    void ty_monocro(QList<QVariant> argments);
    void ty_nega(QList<QVariant> argments);
    void ty_print(QList<QVariant> argments);
    void ty_repaint(QList<QVariant> argments);
    void ty_btndef(QList<QVariant> argments);
    void ty_btntime(QList<QVariant> argments);
    void ty_btntime2(QList<QVariant> argments);
    void ty_btn(QList<QVariant> argments);
    void ty_spbtn(QList<QVariant> argments);
    void ty_cellcheckspbtn(QList<QVariant> argments);
    void ty_exbtn(QList<QVariant> argments);
    void ty_exbtn_d(QList<QVariant> argments);
    void ty_btnwait(QList<QVariant> argments);
    void ty_btnwait2(QList<QVariant> argments);
    void ty_textbtnwait(QList<QVariant> argments);
    void ty_selectbtnwait(QList<QVariant> argments);
    void ty_getbtntimer(QList<QVariant> argments);
    void ty_gettext(QList<QVariant> argments);
    void ty_blt(QList<QVariant> argments);
    void ty_ofscpy(QList<QVariant> argments);
    void ty_selectcolor(QList<QVariant> argments);
    void ty_csel(QList<QVariant> argments);
    void ty_getcselnum(QList<QVariant> argments);
    void ty_cselbtn(QList<QVariant> argments);
    void ty_cselgoto(QList<QVariant> argments);
    void ty_select(QList<QVariant> argments);
    void ty_selgosub(QList<QVariant> argments);
    void ty_selnum(QList<QVariant> argments);
    void ty_br(QList<QVariant> argments);
    void ty_locate(QList<QVariant> argments);
    void ty_textclear(QList<QVariant> argments);
    void ty_puttext(QList<QVariant> argments);
    void ty_clickstr(QList<QVariant> argments);
    void ty_linepage(QList<QVariant> argments);
    void ty_defaultspeed(QList<QVariant> argments);
    void ty_textspeed(QList<QVariant> argments);
    void ty_skipoff(QList<QVariant> argments);
    void ty_quakex(QList<QVariant> argments);
    void ty_quakey(QList<QVariant> argments);
    void ty_quake(QList<QVariant> argments);
    void ty_setcursor(QList<QVariant> argments);
    void ty_abssetcursor(QList<QVariant> argments);
    void ty_textgosub(QList<QVariant> argments);
    void ty_ispage(QList<QVariant> argments);
    void ty_windowback(QList<QVariant> argments);
    void ty_lookbackflush(QList<QVariant> argments);

    // Reset
    void resetGame();

    // Input actions
    void select_action();
    void cancel_action(QObject* sender);
    void up_action();
    void down_action();
    void move_mouse(const QPoint& point);
    void other_action(int code);

    // Printing
    void printing();
    void resumePrinting();
    void automode_select_action();

    // Constants
private:
    static const QString CSEL_LABEL;
    void startAppearTextWindow(int effectNum);
    void processNextChar();
    int scanIntAndLengthFromString(const QString& str, int* value, int* length);
    QString replaceReferenceVariables(const QString& str);
signals:
    void stageUpdated();
    void textPrinted(const QString& text);
    void selectionChanged(int index);
};
//#define VSCREEN_WIDTH 640
//#define VSCREEN_HEIGHT 480

extern int VSCREEN_WIDTH;
extern int VSCREEN_HEIGHT;
#define LD_LOCATE_LEFT "l"
#define LD_LOCATE_CENTER "c"
#define LD_LOCATE_RIGHT "r"
#define CL_LOCATE_ALL "a"
#define WINDOW_EFFECT_NO -2
#define SHADOW_TICKNESS 1
#define SPRITE_MAX 255
#define DEFAULT_TEXTWINDOW "#999999"
#define CSEL_SPRITE_NO 500
#define SPRITE_ALPHA_MAX 255


#endif // TYSTAGEMANAGER_H
