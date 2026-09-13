//
//  TYScriptEngine.h
//  Tukuyomi
//
//  Created by toveta on Thu Jun 07 2001.
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

// スクリプトファイルの読み込み、変換、初期化、内部処理を一手に引き受けるクラスです。
#ifndef TYScriptEngine_h
#define TYScriptEngine_h

#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QList>
#include <QVector>
#include <QMap>
#include <QSet>
#include <QDateTime>
#include <QPoint>

#include "TYScripterValues.h"
#include "TYSaveDataCoding.h"
#include "TYLoopValue.h"

class TYFileLog;
class TYArgment;
class TYScriptEngine;
class TYMainController;
class TYStageManager;
class TYResourceServer;
class TYInputStrController;

struct TYScriptPoint {
    unsigned line;
    unsigned column;
};

class TYScriptEngine : public QObject, public TYSaveDataCoding {
    Q_OBJECT
public:
    static TYScriptEngine* sharedEngine();
    explicit TYScriptEngine(QObject* parent = nullptr);
    bool initWithContentsOfFile(const QString& path);

    // Execution control
    void setController(TYMainController* cont, TYStageManager* manager);
    void runScript();
    bool eval(QList<QVariant>* argments);
    bool breakRun() const { return m_breakRun; }
    void setBreakRun(bool aFlag) { m_breakRun = aFlag; }
    void lineEnd();

    void jump(unsigned target);
    void labeljump(const QString& label);
    void labeljump(const QString& label, int fromLine, bool reverse);
    void jumpToLoopEnd();

    void saveExternalData();
//-(void)saveGlobal;
//-(void)saveLabelLog;

    int runLine() const { return m_runLine; }
    void moveRunLine(int aInt) { m_runLine += aInt; }
    void incRunColumn() { m_runColumn++; }
    void resetRunColumn() { m_runColumn = 0; }

    void registLoopTargetID(QVariant varID, QVariant initVal, QVariant limitVal, QVariant stepVal);
    int splitArgNormal(char* currentchars, QList<QVariant>* cmdArray);
    int splitArgCondition(char** currentcharsptr, bool trueflg);
    bool isMultLineSupport(const QString& command);
    char* nextLine();

    QVariant getIdNoOfArgment(TYArgment* argment);
    QVariant getArrayIdOfArgment(const QString& argment);
    QVariant getValueOfArgment(const QString& argment);
    QString getStringOfArgment(const QString& argment);
    QVariant getEffectNoOfArgments(QList<QVariant>* argments);
    QString stringOfReplaceVars(const QString& aString);

    bool judgefchk(const QString& file);
    bool judgelchk(const QString& label);
    bool judgeCondition(const QString& condition, bool isString);

    void addLabelLog(const QString& string);

    QVariant intNumberWithVarID(QVariant varID);
    QString stringValueWithVarID(QVariant varID);
    QVariant intNumberWithAlias(const QString& aStr);
    QString stringValueWithAlias(const QString& aStr);

    void movRepeat(int repeat_times, QList<QVariant>* argments);

    void fireOfTrap();

    void fireOfTextgosub(const QString& aLabel);

    void enableKidoku(); // 既読スキップをフォロー開始
    void setWatchKidoku(bool aBool);

    void resetGame();

    bool isModeSVGA();
    int scanGlobalValue();

    QVariantMap encodeWithSaveData() override;
    void decodeWithSaveData(const QVariant& aObject) override;

signals:
    void scriptCompleted();
    void breakRequested();

public slots:
    void ty_mov(QList<QVariant>* argments);
    void ty_mov2(QList<QVariant>* argments);
    void ty_mov3(QList<QVariant>* argments);
    void ty_mov4(QList<QVariant>* argments);
    void ty_mov5(QList<QVariant>* argments);
    void ty_mov6(QList<QVariant>* argments);
    void ty_mov7(QList<QVariant>* argments);
    void ty_mov8(QList<QVariant>* argments);
    void ty_mov9(QList<QVariant>* argments);
    void ty_mov10(QList<QVariant>* argments);
    void ty_movl(QList<QVariant>* argments);
    void ty_goto(QList<QVariant>* argments);
    void ty_gosub(QList<QVariant>* argments);
    void ty_return(QList<QVariant>* argments);
    void ty_jumpf(QList<QVariant>* argments);
    void ty_jumpb(QList<QVariant>* argments);
    void ty_tablegoto(QList<QVariant>* argments);
    void ty_skip(QList<QVariant>* argments);
    void ty_next(QList<QVariant>* argments);
    void ty_break(QList<QVariant>* argments);
    void ty_game(QList<QVariant>* argments);
    void ty_end(QList<QVariant>* argments);

    void ty_dim(QList<QVariant>* argments);
    void ty_inc(QList<QVariant>* argments);
    void ty_dec(QList<QVariant>* argments);
    void ty_add(QList<QVariant>* argments);
    void ty_sub(QList<QVariant>* argments);
    void ty_mul(QList<QVariant>* argments);
    void ty_div(QList<QVariant>* argments);
    void ty_mod(QList<QVariant>* argments);
    void ty_cmp(QList<QVariant>* argments);
    void ty_intlimit(QList<QVariant>* argments);

    void ty_atoi(QList<QVariant>* argments);
    void ty_itoa(QList<QVariant>* argments);
    void ty_len(QList<QVariant>* argments);
    void ty_mid(QList<QVariant>* argments);
    void ty_splitstring(QList<QVariant>* argments);
    void ty_stralias(QList<QVariant>* argments);
    void ty_numalias(QList<QVariant>* argments);

    void ty_rnd(QList<QVariant>* argments);
    void ty_rnd2(QList<QVariant>* argments);
    void ty_date(QList<QVariant>* argments);
    void ty_time(QList<QVariant>* argments);
    void ty_resettimer(QList<QVariant>* argments);
    void ty_waittimer(QList<QVariant>* argments);
    void ty_gettimer(QList<QVariant>* argments);

    void ty_nsa(QList<QVariant>* argments);
    void ty_ns2(QList<QVariant>* argments);
    void ty_ns3(QList<QVariant>* argments);
    void ty_nsadir(QList<QVariant>* argments);
    void ty_arc(QList<QVariant>* argments);

    void ty_trap(QList<QVariant>* argments);
    void ty_lr_trap(QList<QVariant>* argments);
    void ty_labellog(QList<QVariant>* argments);

    void ty_input(QList<QVariant>* argments);
    void ty_inputstr(QList<QVariant>* argments);
    void ty_mesbox(QList<QVariant>* argments);

    void ty_globalon(QList<QVariant>* argments);
    void ty_getversion(QList<QVariant>* argments);
    void ty_getreg(QList<QVariant>* argments);
    void ty_getcursorpos(QList<QVariant>* argments);
    void ty_loadgame(QList<QVariant>* argments);
    void ty_savegame(QList<QVariant>* argments);

    void ty_log(QList<QVariant>* argments);

    void ty_print(QList<QVariant>* argments);

#if YYDEBUG == 1
    void ty_yydebug(QList<QVariant>* argments);
#endif

private:
    TYMainController* m_controller;
    TYStageManager* m_stageManager;

    bool m_breakRun;
    QStringList m_scriptArray;
    QByteArray m_currentData;
    unsigned int m_runColumn;
    unsigned int m_runLine;
    TYScriptPoint m_savePoint;
    char m_newLineCode[3];
    QList<QVariant> m_returnStack;
    QSet<QString> m_multiLineCmdStringSet;
    QList<QVariant> m_loopStack;

    QMap<QString, QVariant> m_numAliases;
    QMap<QString, QString> m_strAliases;
    TYScripterValues* m_scripterValues;
    bool m_globalOn;
    QString m_nsaDir;

    TYFileLog* m_labelLog;
    QDateTime m_timerStartDate;

    QString m_labelWithTrap;
    bool m_fireOfTrap;

    QMap<QString, QVariant> m_labelDict;

    QByteArray m_kidokuData;
    unsigned char* m_kidokuptr;
    bool m_isWatchKidoku;

    TYScriptPoint m_textgosubPoint;
    int m_textgosubPointIndex;

    QSet<QString> m_undefinedComSet;
};

#define DEFALUT_NSA_ARCHIVE QString("arc%1.nsa")
#define GLOBAL_SAVEFILE_NAME QString("gloval.sav")
#define MULTILINE_CMD_STRINGS QStringList() << "select" << "selnum" << "selgosub" << "csel"
#define LABELLOG_FILENAME QString("NScrllog.dat")
#define KIDOKU_FILENAME QString("ccs_kidoku.dat")

extern const QString TYNScriptRunTimeErrorException;
extern const QString TYNScriptSyntaxErrorException;

// Helper function declarations
TYScriptPoint TYMakeScriptPoint(unsigned line, unsigned column);

#endif // TYScriptEngine_h
