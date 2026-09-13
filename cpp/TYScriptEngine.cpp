//
//  TYScriptEngine.m
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

#include "TYScriptEngine.h"
#include "TYMainController.h"
#include "TYResourceServer.h"
#include "TY-NSStringAddition.h"
#include "TYStageManager.h"
#include "TYMiscUtil.h"
#include "TYFileLog.h"
#include "TYEnviroment.h"
#include "TYInputStrController.h"
#include "TYArgment.h"
#include "TYStringVarArgment.h"
#include "TYIntVarArgment.h"
#include "TYConstStringArgment.h"
#include "TYNsaArchiver.h"
#include "TYScripterValues.h"

#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include <QDataStream>
#include <QScroller>

// Constants
const QString TYNScriptRunTimeErrorException = QString("TYNScriptRunTimeErrorException");
const QString TYNScriptSyntaxErrorException = QString("TYNScriptSyntaxErrorException");

// gnscripterからコピペ(ぉ
#define myisdigit(c) ((c) >= '0' && (c) <= '9')
#define myishexdigit(c) (myisdigit(c) || (c) >= 'a' && (c) <= 'f' || (c) >= 'A' && (c) <= 'F')
#define myisalpha(c) ((c) >= 'a' && (c) <= 'z' || (c) >= 'A' && (c) <= 'Z' || (c) == '_')
#define myisalnum(c) (myisdigit(c) || myisalpha(c))
#define myvarchar(c) (myisalnum(c) || (c) == '%' || (c) == '$' || (c) == '[' || (c) == ']' || (c) == '?')

#define getbit(b,i) (*((b)+((i)>>3)) & (1 << ((i) & 0x7)))

#define MAX_LINE_LENGTH 1024

static TYScriptEngine* sharedEngine = nullptr;

static const char* newLineCodeUnix = "\n";
static const char* newLineCodeWin = "\r\n";
static const char* newLineCodeMac = "\r";

static int slideColumn(char** ptr, unsigned toColumn);
static int get_item(char** ptr, char* buffer);
static int get_vararg(char** ptr, char* buffer);
static void space_skip(char** ptr);

// Argument context enumeration
enum TYArgContext {
    TYNoArgContext = -1,
    TYUndefinedArgContext,
    TYOperandArgContext,
    TYStringArgContext,
    TYIntergerArgContext,
    TYSeparaterArgContext,
    TYChainArgContext
};

// Helper function implementations
static int slideColumn(char** ptr, unsigned toColumn)
{
    unsigned column = 0;
    bool escape = false;
    
    while (column < toColumn) {
        if (**ptr == '\0') {
            return 1; // line up
        }
        if (**ptr == '"') {
            escape = escape ? false : true;
        }
        if ((!escape) && (**ptr == ':')) {
            column++;
        }
        (*ptr)++;
    }
    return 0;
}

static int get_item(char** ptr, char* buffer)
{
    int context = TYUndefinedArgContext;

    if ((**ptr == '%') || (**ptr == '?')) {
        context = TYIntergerArgContext;
        get_vararg(ptr, buffer);
        return context;
    } else if (**ptr == '$') {
        context = TYStringArgContext;
        get_vararg(ptr, buffer);
        return context;
    } else if (**ptr == '"') {
        context = TYStringArgContext;
        *buffer = **ptr;
        buffer++;
        (*ptr)++;
        while (**ptr && **ptr != '"') {
            *buffer = **ptr;
            (*ptr)++;
            buffer++;
        }
        *buffer = **ptr;
        buffer++;
        (*ptr)++;
        *buffer = '\0';
        return context;
    } else if (**ptr == '(') {
        char strbuffer[MAX_LINE_LENGTH + 1];
        context = TYStringArgContext;

        *buffer = **ptr;
        (*ptr)++;
        buffer++;
        while (**ptr && isspace(**ptr)) {
            *buffer = **ptr;
            (*ptr)++;
            buffer++;
        }
        get_item(ptr, strbuffer);
        *buffer = '\0';
        strcat(buffer, strbuffer);
        buffer += strlen(strbuffer);
        while (**ptr && isspace(**ptr)) {
            *buffer = **ptr;
            (*ptr)++;
            buffer++;
        }
        *buffer = **ptr;
        (*ptr)++;
        buffer++;
        // second
        while (**ptr && isspace(**ptr)) {
            *buffer = **ptr;
            (*ptr)++;
            buffer++;
        }
        get_item(ptr, strbuffer);
        *(buffer) = '\0';
        strcat(strbuffer, " ");
        strcat(buffer, strbuffer);
        buffer += strlen(strbuffer);
        // third
        while (**ptr && isspace(**ptr)) {
            *buffer = **ptr;
            (*ptr)++;
            buffer++;
        }
        get_item(ptr, strbuffer);
        *(buffer) = '\0';
        strcat(buffer, strbuffer);

        return context;
    } else if (isdigit(**ptr) || (**ptr == '-')) {
        context = TYIntergerArgContext;
        *buffer = **ptr;
        (*ptr)++;
        buffer++;
        while (**ptr && isdigit(**ptr)) {
            *buffer = **ptr;
            (*ptr)++;
            buffer++;
        }
        *buffer = '\0';
        return context;
    } else if (isalpha(**ptr)) {
        context = TYUndefinedArgContext;
        while (**ptr && myisalnum(**ptr)) {
            *buffer = **ptr;
            (*ptr)++;
            buffer++;
        }
        *buffer = '\0';
        return context;
    } else if (**ptr && strchr("!<>=", **ptr)) {
        context = TYOperandArgContext;
        while (**ptr && strchr("!<>=", **ptr)) {
            *buffer = **ptr;
            (*ptr)++;
            buffer++;
        }
        *buffer = '\0';
        return context;
    } else if (**ptr == ',') {
        context = TYSeparaterArgContext;
        *buffer = **ptr;
        (*ptr)++;
        buffer++;
        *buffer = '\0';
        return context;
    } else if (**ptr == '&') {
        context = TYChainArgContext;
        while (**ptr && **ptr == '&') {
            *buffer = **ptr;
            (*ptr)++;
            buffer++;
        }
        *buffer = '\0';
        return context;
    } else if (**ptr == '*' || **ptr == '#') {
        context = TYStringArgContext;
        *buffer = **ptr;
        (*ptr)++;
        buffer++;
        while (**ptr && myisalnum(**ptr)) {
            *buffer = **ptr;
            (*ptr)++;
            buffer++;
        }
        *buffer = '\0';
        return context;
    }

    return TYNoArgContext;
}

static int get_vararg(char** ptr, char* buffer)
{
    while (**ptr) {
        if (isalnum(**ptr)) {
            while (**ptr && (myvarchar(**ptr))) {
                *buffer = **ptr;
                buffer++;
                (*ptr)++;
            }
            *buffer = '\0';
            return 0;
        } else {
            *buffer = **ptr;
            buffer++;
            (*ptr)++;
        }
    }

    *buffer = '\0';
    return 0;
}

static void space_skip(char** ptr)
{
    while (**ptr && isspace(**ptr))
        (*ptr)++;
}

TYScriptPoint TYMakeScriptPoint(unsigned line, unsigned column)
{
    TYScriptPoint scPoint;
    scPoint.line = line;
    scPoint.column = column;
    return scPoint;
}

// Static shared engine accessor
TYScriptEngine* TYScriptEngine::sharedEngine()
{
    return sharedEngine;
}

// Constructor
TYScriptEngine::TYScriptEngine(QObject* parent)
    : QObject(parent)
    , m_controller(nullptr)
    , m_stageManager(nullptr)
    , m_breakRun(false)
    , m_runColumn(0)
    , m_runLine(0)
    , m_scripterValues(nullptr)
    , m_globalOn(false)
    , m_labelLog(nullptr)
    , m_fireOfTrap(false)
    , m_kidokuptr(nullptr)
    , m_isWatchKidoku(false)
    , m_textgosubPointIndex(-1)
{
    m_newLineCode[0] = '\0';
}

// Initialize with script file
bool TYScriptEngine::initWithContentsOfFile(const QString& path)
{
    QFileManager fileManager;
    QByteArray scriptData;
    QString truePath;
    QByteArray addData;
    QStringList filesArray;
    unsigned char* encodedCharptr = nullptr;
    unsigned scriptLength = 0;
    const unsigned char* stringSourceptr = nullptr;
    bool scriptMode = false; // mode_single = false, mode_multi = true

    // Check for multi-script mode (look for 0.txt)
    filesArray = fileManager.entryList(path);
    for (int i = 0; i < filesArray.size(); i++) {
        if (filesArray.at(i) == QString("0.txt")) {
            scriptMode = true;
            break;
        }
    }

    if (!scriptMode) {
        truePath = path + QDir::separator() + QString("nscript.dat");

        QFile file(truePath);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "Script File is not Found";
            return false;
        }
        scriptData = file.readAll();
        file.close();

        if (scriptData.isEmpty()) {
            qDebug() << "Script File is empty";
            return false;
        }

        encodedCharptr = (unsigned char*)scriptData.data();

        // Decode XOR with 132
        scriptLength = scriptData.size();
        for (unsigned i = 0; i < scriptLength; i++, encodedCharptr++) {
            *encodedCharptr = (*encodedCharptr) ^ 132;
        }
        stringSourceptr = (const unsigned char*)scriptData.constData();
    } else {
        // Multi-script mode: read 0.txt, 1.txt, etc.
        scriptData.clear();
        int i = 0;

        while (true) {
            QString filePath = path + QDir::separator() + QString::number(i) + QString(".txt");
            QFile txtFile(filePath);
            if (txtFile.open(QIODevice::ReadOnly)) {
                addData = txtFile.readAll();
                txtFile.close();
                scriptData.append(addData);
                scriptData.append("\r\n");
            } else {
                break;
            }
            i++;
        }
        scriptLength = scriptData.size();
        stringSourceptr = (const unsigned char*)scriptData.constData();
    }

    // Detect line ending
    for (unsigned i = 0; i < scriptLength; i++) {
        if (stringSourceptr[i] == '\n') {
            strcpy(m_newLineCode, newLineCodeUnix);
            break;
        } else if (stringSourceptr[i] == '\r') {
            if (stringSourceptr[i + 1] == '\n') {
                strcpy(m_newLineCode, newLineCodeWin);
                break;
            } else {
                strcpy(m_newLineCode, newLineCodeMac);
            }
        }
    }

    // Convert to string using Shift-JIS encoding
    {
        QTextCodec* codec = QTextCodec::codecForName("Shift-JIS");
        if (codec) {
            QString str = codec->toUnicode(scriptData.constData(), scriptData.size());
            m_scriptArray = str.split(QString::fromLatin1(m_newLineCode), QString::SkipEmptyParts);
        }
    }

    // Build label dictionary
    {
        QSet<QChar> varNameChars; // Simplified - would need actual character set
        int line = 0;
        for (const QString& lineStr : m_scriptArray) {
            QString trimmed = lineStr.trimmed();
            if (trimmed.startsWith('*')) {
                QString labelName = trimmed.mid(1).toLower();
                m_labelDict[QString("*") + labelName] = QVariant(line);
            }
            line++;
        }
    }

    // Initialize other structures
    m_numAliases.clear();
    m_strAliases.clear();
    m_scripterValues = TYScripterValues::scripterValues();
    m_multiLineCmdStringSet = QSet<QString>() << "select" << "selnum" << "selgosub" << "csel";
    m_returnStack.clear();
    m_timerStartDate = QDateTime::currentDateTime();
    m_loopStack.clear();
    m_textgosubPointIndex = -1;
    m_undefinedComSet.clear();

    // Check for global value
    int value = scanGlobalValue();
    if (value > 0) {
        qDebug() << "change global ID bottom =" << value;
        TYScripterValues::setMinGlobalNo(value);
    }

    // Load global values
    m_scripterValues->loadGlobalValues(getNScrRootDirectory() + GLOBAL_SAVEFILE_NAME);

    // Jump to define label
    labeljump(QString("*define"));

    // Set random seed
    QDateTime cDate = QDateTime::currentDateTime();
    unsigned int seed = (cDate.time().hour() * 60 + cDate.time().minute()) * 60 + cDate.time().second();
    qsrand(seed);

    // Store as shared
    sharedEngine = this;

    return true;
}

bool TYScriptEngine::isModeSVGA()
{
    if (m_scriptArray.isEmpty()) return false;
    
    QString firstLine = m_scriptArray.at(0).trimmed();
    return firstLine.contains(QString(";mode800"));
}

int TYScriptEngine::scanGlobalValue()
{
    if (m_scriptArray.isEmpty()) return 0;
    
    QString firstLine = m_scriptArray.at(0);
    int pos = firstLine.indexOf(QString("value"));
    if (pos == -1) {
        return 0;
    }
    
    QString afterValue = firstLine.mid(pos + 5).trimmed();
    int value = 0;
    for (int i = 0; i < afterValue.size() && afterValue[i].isDigit(); i++) {
        value = value * 10 + afterValue[i].digitValue();
    }
    
    return value;
}

QVariantMap TYScriptEngine::encodeWithSaveData()
{
    QVariantMap aDict;
    QByteArray valueData;
    QVariant pointValue;

    valueData = m_scripterValues->encodeWithSaveData();

    // Adjust save point based on status
    if (m_controller && m_controller->status() == TYSelectStatus) {
        if (m_savePoint.column == 0) {
            m_savePoint.line--;
        }
    } else if (m_controller && m_controller->status() == TYScriptRunningStatus) {
        m_savePoint.column++;
    }

    pointValue = QVariant::fromValue(m_savePoint);

    if (m_textgosubPointIndex == -1) {
        aDict[TYLocalValuesSaveData] = valueData;
        aDict[TYScriptPointSaveData] = pointValue;
        aDict[TYReturnStackSaveData] = QVariant::fromValue(m_returnStack);
    } else {
        aDict[TYLocalValuesSaveData] = valueData;
        aDict[TYScriptPointSaveData] = pointValue;
        aDict[TYReturnStackSaveData] = QVariant::fromValue(m_returnStack);
        aDict[TYTextgosubIndexSaveData] = m_textgosubPointIndex;
    }

    return aDict;
}

void TYScriptEngine::decodeWithSaveData(const QVariant& aObject)
{
    QVariantMap aMap = aObject.toMap();

    QVariant valueDataVar = aMap.value(TYLocalValuesSaveData);
    if (valueDataVar.isValid()) {
        m_scripterValues->decodeWithSaveData(valueDataVar);
    }

    QVariant tmp = aMap.value(TYReturnStackSaveData);
    if (tmp.isValid()) {
        m_returnStack = tmp.toList();
    }

    tmp = aMap.value(TYScriptPointSaveData);
    if (tmp.isValid()) {
        m_savePoint = tmp.value<TYScriptPoint>();
        m_runLine = m_savePoint.line;
        m_runColumn = m_savePoint.column;
    }

    tmp = aMap.value(TYTextgosubIndexSaveData);
    if (tmp.isValid()) {
        m_textgosubPointIndex = tmp.toInt();
    } else {
        m_textgosubPointIndex = -1;
    }
}

void TYScriptEngine::resetGame()
{
    m_scripterValues->resetGame();
    m_returnStack.clear();
    m_textgosubPointIndex = -1;
    labeljump(QString("*start"));
}

void TYScriptEngine::setController(TYMainController* cont, TYStageManager* manager)
{
    m_controller = cont;
    m_stageManager = manager;
}

void TYScriptEngine::runScript()
{
    QString currentString;
    const char* startptr;
    char* currentchars;
    char command[MAX_LINE_LENGTH];

    m_breakRun = false;

    if (m_controller) {
        m_controller->setStatus(TYScriptRunningStatus);
    }

    // Main script loop
    while (true) {
        // Trap handling
        if (!m_labelWithTrap.isNull() && m_fireOfTrap) {
            labeljump(m_labelWithTrap);
            if (m_controller) {
                m_controller->setTrap(false);
            }
            m_fireOfTrap = false;
            m_labelWithTrap.clear();
        }

        // Advance line if at column 0
        if (m_runColumn == 0) {
            m_runLine++;
        }

        if (m_runLine >= (unsigned)m_scriptArray.size()) {
            break;
        }

        currentString = m_scriptArray.at(m_runLine);
        m_currentData = currentString.toLocal8Bit();
        currentchars = m_currentData.data();

        // Line length check
        if (m_currentData.size() > MAX_LINE_LENGTH) {
            qDebug() << "too long line. omit data over" << MAX_LINE_LENGTH;
            currentchars[MAX_LINE_LENGTH - 1] = '\0';
        }
        startptr = currentchars;

        // Handle column position
        if (m_runColumn > 0) {
            if (slideColumn(&currentchars, m_runColumn)) {
                m_runLine++;
                m_runColumn = 0;
                if (m_runLine >= (unsigned)m_scriptArray.size()) {
                    break;
                }
                currentString = m_scriptArray.at(m_runLine);
                m_currentData = currentString.toLocal8Bit();
                currentchars = m_currentData.data();
                startptr = currentchars;
            }
        }

        m_savePoint.line = m_runLine;
        m_savePoint.column = m_runColumn;

        // Parse using parser
        TYParserSetCString(currentchars);
        yyparse();

        if (m_breakRun) {
            break;
        }
    }
}

void TYScriptEngine::ty_print(QList<QVariant>* argments)
{
    if (argments->size() < 2) return;
    QString aStr = argments->at(1).toString();

    // Kidoku tracking
    if (m_kidokuptr) {
        if (m_isWatchKidoku) {
            if (!(*(m_kidokuptr + (m_runLine >> 3)) & (1 << (m_runLine & 0x7)))) {
                m_stageManager->setSkipStatus(TYNoSkip);
                m_isWatchKidoku = false;
                *(m_kidokuptr + (m_runLine >> 3)) |= 1 << (m_runLine & 0x7);
            }
        } else {
            *(m_kidokuptr + (m_runLine >> 3)) |= 1 << (m_runLine & 0x7);
        }
    }

    m_stageManager->textPrint(aStr);
}

bool TYScriptEngine::eval(QList<QVariant>* argments)
{
    if (argments->isEmpty()) return false;

    QString cmdName = argments->at(0).toString().toLower();
    QString methodName = QString("ty_") + cmdName;

    // Try to find and call the method
    // In Qt, we use signal/slot or direct method calls
    // For script engine, we dispatch to specific handlers

    if (methodName == "ty_mov") {
        ty_mov(argments);
    } else if (methodName == "ty_mov2") {
        ty_mov2(argments);
    } else if (methodName == "ty_mov3") {
        ty_mov3(argments);
    } else if (methodName == "ty_mov4") {
        ty_mov4(argments);
    } else if (methodName == "ty_mov5") {
        ty_mov5(argments);
    } else if (methodName == "ty_mov6") {
        ty_mov6(argments);
    } else if (methodName == "ty_mov7") {
        ty_mov7(argments);
    } else if (methodName == "ty_mov8") {
        ty_mov8(argments);
    } else if (methodName == "ty_mov9") {
        ty_mov9(argments);
    } else if (methodName == "ty_mov10") {
        ty_mov10(argments);
    } else if (methodName == "ty_movl") {
        ty_movl(argments);
    } else if (methodName == "ty_goto") {
        ty_goto(argments);
    } else if (methodName == "ty_gosub") {
        ty_gosub(argments);
    } else if (methodName == "ty_return") {
        ty_return(argments);
    } else if (methodName == "ty_jumpf") {
        ty_jumpf(argments);
    } else if (methodName == "ty_jumpb") {
        ty_jumpb(argments);
    } else if (methodName == "ty_tablegoto") {
        ty_tablegoto(argments);
    } else if (methodName == "ty_skip") {
        ty_skip(argments);
    } else if (methodName == "ty_next") {
        ty_next(argments);
    } else if (methodName == "ty_break") {
        ty_break(argments);
    } else if (methodName == "ty_game") {
        ty_game(argments);
    } else if (methodName == "ty_end") {
        ty_end(argments);
    } else if (methodName == "ty_dim") {
        ty_dim(argments);
    } else if (methodName == "ty_inc") {
        ty_inc(argments);
    } else if (methodName == "ty_dec") {
        ty_dec(argments);
    } else if (methodName == "ty_add") {
        ty_add(argments);
    } else if (methodName == "ty_sub") {
        ty_sub(argments);
    } else if (methodName == "ty_mul") {
        ty_mul(argments);
    } else if (methodName == "ty_div") {
        ty_div(argments);
    } else if (methodName == "ty_mod") {
        ty_mod(argments);
    } else if (methodName == "ty_cmp") {
        ty_cmp(argments);
    } else if (methodName == "ty_intlimit") {
        ty_intlimit(argments);
    } else if (methodName == "ty_atoi") {
        ty_atoi(argments);
    } else if (methodName == "ty_itoa") {
        ty_itoa(argments);
    } else if (methodName == "ty_len") {
        ty_len(argments);
    } else if (methodName == "ty_mid") {
        ty_mid(argments);
    } else if (methodName == "ty_splitstring") {
        ty_splitstring(argments);
    } else if (methodName == "ty_stralias") {
        ty_stralias(argments);
    } else if (methodName == "ty_numalias") {
        ty_numalias(argments);
    } else if (methodName == "ty_rnd") {
        ty_rnd(argments);
    } else if (methodName == "ty_rnd2") {
        ty_rnd2(argments);
    } else if (methodName == "ty_date") {
        ty_date(argments);
    } else if (methodName == "ty_time") {
        ty_time(argments);
    } else if (methodName == "ty_resettimer") {
        ty_resettimer(argments);
    } else if (methodName == "ty_waittimer") {
        ty_waittimer(argments);
    } else if (methodName == "ty_gettimer") {
        ty_gettimer(argments);
    } else if (methodName == "ty_nsa") {
        ty_nsa(argments);
    } else if (methodName == "ty_ns2") {
        ty_ns2(argments);
    } else if (methodName == "ty_ns3") {
        ty_ns3(argments);
    } else if (methodName == "ty_nsadir") {
        ty_nsadir(argments);
    } else if (methodName == "ty_arc") {
        ty_arc(argments);
    } else if (methodName == "ty_trap") {
        ty_trap(argments);
    } else if (methodName == "ty_lr_trap") {
        ty_lr_trap(argments);
    } else if (methodName == "ty_labellog") {
        ty_labellog(argments);
    } else if (methodName == "ty_input") {
        ty_input(argments);
    } else if (methodName == "ty_inputstr") {
        ty_inputstr(argments);
    } else if (methodName == "ty_mesbox") {
        ty_mesbox(argments);
    } else if (methodName == "ty_globalon") {
        ty_globalon(argments);
    } else if (methodName == "ty_getversion") {
        ty_getversion(argments);
    } else if (methodName == "ty_getreg") {
        ty_getreg(argments);
    } else if (methodName == "ty_getcursorpos") {
        ty_getcursorpos(argments);
    } else if (methodName == "ty_loadgame") {
        ty_loadgame(argments);
    } else if (methodName == "ty_savegame") {
        ty_savegame(argments);
    } else if (methodName == "ty_log") {
        ty_log(argments);
    } else if (methodName == "ty_print") {
        ty_print(argments);
    }
#if YYDEBUG == 1
    else if (methodName == "ty_yydebug") {
        ty_yydebug(argments);
    }
#endif
    else {
        // Check for vXXXX or dvXXXX patterns
        if (cmdName.startsWith("dv") || cmdName.startsWith("v")) {
            QString numStr = cmdName.mid(1);
            QList<QVariant> newArgs;
            newArgs.append(cmdName.left(1));
            newArgs.append(numStr);
            if (m_controller) {
                // Delegate to controller
            }
        } else {
            if (!m_undefinedComSet.contains(cmdName)) {
                qDebug() << "Undefined script" << cmdName << "at LINE" << (m_runLine + 1);
                m_undefinedComSet.insert(cmdName);
            }
            return false;
        }
    }

    return true;
}

bool TYScriptEngine::breakRun()
{
    return m_breakRun;
}

void TYScriptEngine::setBreakRun(bool aFlag)
{
    m_breakRun = aFlag;
}

void TYScriptEngine::lineEnd()
{
    // Column increment handled elsewhere
}

int TYScriptEngine::splitArgNormal(char* currentchars, QList<QVariant>* cmdArray)
{
    char* buffer = currentchars;
    char command[MAX_LINE_LENGTH];
    int context;
    bool multiLinePossible = true;

    space_skip(&buffer);
    context = get_item(&buffer, command);

    while (context != TYNoArgContext) {
        cmdArray->append(QString::fromLocal8Bit(command));
        space_skip(&buffer);
        context = get_item(&buffer, command);
        if (context == TYSeparaterArgContext) {
            space_skip(&buffer);
            context = get_item(&buffer, command);
        } else {
            multiLinePossible = false;
            break;
        }
    }

    if (*buffer == ':') {
        m_runColumn++;
    } else {
        if (multiLinePossible && isMultLineSupport(cmdArray->first().toString())) {
            char* nextLineData = nextLine();
            splitArgNormal(nextLineData, cmdArray);
        } else {
            m_runColumn = 0;
        }
    }

    return 0;
}

bool TYScriptEngine::isMultLineSupport(const QString& command)
{
    return m_multiLineCmdStringSet.contains(command);
}

char* TYScriptEngine::nextLine()
{
    m_runLine++;
    if (m_runLine >= (unsigned)m_scriptArray.size()) {
        return nullptr;
    }
    QString lineStr = m_scriptArray.at(m_runLine);
    m_currentData = lineStr.toLocal8Bit();
    return m_currentData.data();
}

void TYScriptEngine::registLoopTargetID(QVariant varID, QVariant initVal, QVariant limitVal, QVariant stepVal)
{
    TYLoopStruct loop;
    qDebug() << "push for stack at" << m_runLine;

    // Set initial value
    m_scripterValues->setIntValue(varID, initVal.toInt());

    // Create loop struct and push to stack
    loop = TYMakeLoopStruct(TYMakeScriptPoint(m_runLine, m_runColumn), varID, limitVal.toInt(), stepVal.toInt());
    TYLoopValue* loopValue = new TYLoopValue(loop);
    m_loopStack.append(QVariant::fromValue(loopValue));
}

static int chkNext(char** ptr, int stColumn);

static int chkNext(char** ptr, int stColumn)
{
    int column;
    int result = -1;

    for (column = stColumn; !slideColumn(ptr, column); column++) {
        space_skip(ptr);
        if (strncmp("next", *ptr, 4) == 0) {
            result = column;
            break;
        }
    }

    return result;
}

void TYScriptEngine::jumpToLoopEnd()
{
    char* ptr = nullptr;
    int column = m_runColumn + 1;

    while (true) {
        if (m_runLine >= (unsigned)m_scriptArray.size()) break;
        ptr = m_scriptArray.at(m_runLine).toLocal8Bit().data();
        column = chkNext(&ptr, column);
        if (column >= 0) {
            m_runColumn = column + 1;
            break;
        } else {
            m_runLine++;
            column = 0;
        }
    }
}

int TYScriptEngine::splitArgCondition(char** currentcharsptr, bool trueflg)
{
    int k, count;
    QList<QString> conditions;
    char buffer[MAX_LINE_LENGTH];
    char sentence[MAX_LINE_LENGTH];
    int context = TYChainArgContext;
    bool judgeResult;

    conditions.clear();

    while (context == TYChainArgContext) {
        space_skip(currentcharsptr);
        context = get_item(currentcharsptr, buffer);
        strcpy(sentence, buffer);
        if (context == TYIntergerArgContext) {
            space_skip(currentcharsptr);
            context = get_item(currentcharsptr, buffer);
            if (context != TYOperandArgContext) {
                qDebug() << "invalid argment [" << buffer << "]. it needs operand here";
                return false;
            }
            strcat(sentence, buffer);

            space_skip(currentcharsptr);
            context = get_item(currentcharsptr, buffer);
            if ((context != TYIntergerArgContext) && (context != TYUndefinedArgContext)) {
                qDebug() << "invalid argment [" << buffer << "]. it needs interger here";
                return false;
            }
            strcat(sentence, buffer);
            conditions.append(QString::fromLocal8Bit(sentence));

        } else if (context == TYStringArgContext) {
            conditions.append(QString("string"));
            space_skip(currentcharsptr);
            context = get_item(currentcharsptr, buffer);
            if (context != TYOperandArgContext) {
                qDebug() << "invalid argment [" << buffer << "]. it needs operand here";
                return false;
            }
            strcat(sentence, buffer);

            space_skip(currentcharsptr);
            context = get_item(currentcharsptr, buffer);
            if ((context != TYStringArgContext) && (context != TYUndefinedArgContext)) {
                qDebug() << "invalid argment [" << buffer << "]. it needs string here";
                return false;
            }
            strcat(sentence, buffer);
            conditions.append(QString::fromLocal8Bit(sentence));
        } else if (context == TYUndefinedArgContext) {
            if ((strcmp(buffer, "fchk") == 0) || (strcmp(buffer, "lchk") == 0)) {
                conditions.append(QString::fromLocal8Bit(buffer));
                space_skip(currentcharsptr);
                context = get_item(currentcharsptr, buffer);
                if ((context == TYStringArgContext) || (context == TYUndefinedArgContext)) {
                    conditions.append(QString::fromLocal8Bit(buffer));
                } else {
                    qDebug() << "invalid argment [" << buffer << "]. it needs string here";
                    return false;
                }
            } else {
                space_skip(currentcharsptr);
                context = get_item(currentcharsptr, buffer);
                if (context != TYOperandArgContext) {
                    qDebug() << "invalid argment [" << buffer << "]. it needs operand here";
                    return false;
                }
                strcat(sentence, buffer);

                space_skip(currentcharsptr);
                context = get_item(currentcharsptr, buffer);
                if (context != TYOperandArgContext) {
                    qDebug() << "invalid argment [" << buffer << "]. it needs interger here";
                    return false;
                }
                strcat(sentence, buffer);
                conditions.append(QString::fromLocal8Bit(sentence));
            }
        } else {
            qDebug() << "invalid argment [" << buffer << "]. it needs interger or \"fchk\" or \"lchk\" here";
            return false;
        }

        space_skip(currentcharsptr);
        if (**currentcharsptr == '&') {
            context = get_item(currentcharsptr, buffer);
        } else {
            break;
        }
    }

    // Evaluate conditions
    for (k = 0, count = conditions.size(); k < count; k++) {
        if (conditions.at(k) == QString("fchk")) {
            k++;
            judgeResult = judgefchk(conditions.at(k));
        } else if (conditions.at(k) == QString("lchk")) {
            k++;
            judgeResult = judgelchk(conditions.at(k));
        } else if (conditions.at(k) == QString("string")) {
            k++;
            judgeResult = judgeCondition(conditions.at(k), true);
        } else {
            judgeResult = judgeCondition(conditions.at(k), false);
        }

        if (judgeResult != trueflg) {
            return false;
        }
    }

    return true;
}

bool TYScriptEngine::judgefchk(const QString& file)
{
    return TYResourceServer::sharedServer()->fchk(getStringOfArgment(file));
}

bool TYScriptEngine::judgelchk(const QString& label)
{
    return m_labelLog->isRead(getStringOfArgment(label));
}

void TYScriptEngine::addLabelLog(const QString& string)
{
    if (m_labelLog) {
        m_labelLog->addLog(string);
    }
}

bool TYScriptEngine::judgeCondition(const QString& condition, bool aBool)
{
    char valueString1[MAX_LINE_LENGTH];
    char valueString2[MAX_LINE_LENGTH];
    char cond[3];
    QByteArray conditionBytes = condition.toLocal8Bit();
    const char* conditionchars = conditionBytes.constData();
    int value1, value2;
    const char* operandchars = "!<>=";
    int i, j;

    // Extract left operand
    for (i = 0, j = 0; (conditionchars[i]) && (strchr(operandchars, conditionchars[i]) == nullptr); i++, j++) {
        valueString1[j] = conditionchars[i];
    }
    valueString1[j] = '\0';

    // Extract operator
    for (j = 0; (conditionchars[i]) && (strchr(operandchars, conditionchars[i]) != nullptr); i++, j++) {
        cond[j] = conditionchars[i];
    }
    cond[j] = '\0';

    // Extract right operand
    for (j = 0; (conditionchars[i]); i++, j++) {
        valueString2[j] = conditionchars[i];
    }
    valueString2[j] = '\0';

    if (aBool) {
        QString string1, string2;
        string1 = getStringOfArgment(QString::fromLocal8Bit(valueString1));
        string2 = getStringOfArgment(QString::fromLocal8Bit(valueString2));

        if ((strcmp(cond, "==") == 0) || (strcmp(cond, "=") == 0)) {
            return string1 == string2;
        } else if ((strcmp(cond, "!=") == 0) || (strcmp(cond, "<>") == 0)) {
            return string1 != string2;
        } else if (strcmp(cond, ">") == 0) {
            return string1 > string2;
        } else if (strcmp(cond, "<") == 0) {
            return string1 < string2;
        } else if (strcmp( cond, ">=") == 0) {
            return string1 >= string2;
        } else if (strcmp(cond, "<=") == 0) {
            return string1 <= string2;
        } else {
            qDebug() << "It's illegal operand![" << conditionchars << "]";
            return false;
        }
    } else {
        value1 = getValueOfArgment(QString::fromLocal8Bit(valueString1)).toInt();
        value2 = getValueOfArgment(QString::fromLocal8Bit(valueString2)).toInt();

        if ((strcmp(cond, "==") == 0) || (strcmp(cond, "=") == 0)) {
            return value1 == value2;
        } else if ((strcmp(cond, "!=") == 0) || (strcmp(cond, "<>") == 0)) {
            return value1 != value2;
        } else if (strcmp(cond, ">") == 0) {
            return value1 > value2;
        } else if (strcmp(cond, "<") == 0) {
            return value1 < value2;
        } else if (strcmp(cond, ">=") == 0) {
            return value1 >= value2;
        } else if (strcmp(cond, "<=") == 0) {
            return value1 <= value2;
        } else {
            qDebug() << "It's illegal operand![" << conditionchars << "]";
            return false;
        }
    }
}

void TYScriptEngine::jump(unsigned target)
{
    m_runLine = target;
    m_runColumn = 0;
}

void TYScriptEngine::labeljump(const QString& label)
{
    QString lookupLabel = label.toLower();

    if (m_labelDict.contains(lookupLabel)) {
        if (m_labelLog) {
            m_labelLog->addLog(label);
        }
        m_runLine = m_labelDict.value(lookupLabel).toUInt();
        m_runColumn = 0;
        return;
    } else {
        qDebug() << "Not Found Label:" << label;
        // Could throw exception here
    }
}

void TYScriptEngine::labeljump(const QString& label, int fromLine, bool reverse)
{
    unsigned l;
    unsigned length;
    QString str;

    if (m_labelLog) {
        m_labelLog->addLog(label);
    }

    if (!reverse) {
        length = m_scriptArray.size();

        for (l = fromLine; l < length; l++) {
            str = m_scriptArray.at(l);
            if (str.isEmpty()) {
                continue;
            }
            QString trimmed = str.trimmed();
            if (trimmed.startsWith(label)) {
                QString afterLabel = trimmed.mid(label.size()).trimmed();
                if (afterLabel.isEmpty() || afterLabel[0].isSpace()) {
                    m_runLine = l;
                    m_runColumn = 0;
                    return;
                }
            }
        }
    } else {
        for (l = fromLine; l >= 0; l--) {
            if (l >= (unsigned)m_scriptArray.size()) {
                continue;
            }
            str = m_scriptArray.at(l);
            if (str.isEmpty()) {
                continue;
            }
            QString trimmed = str.trimmed();
            if (trimmed.startsWith(label)) {
                QString afterLabel = trimmed.mid(label.size()).trimmed();
                if (afterLabel.isEmpty() || afterLabel[0].isSpace()) {
                    m_runLine = l;
                    m_runColumn = 0;
                    return;
                }
            }
        }
    }

    qDebug() << "Not Found Label:" << label;
}

QVariant TYScriptEngine::getValueOfArgment(const QString& argment)
{
    return argment.toInt();
}

QString TYScriptEngine::getStringOfArgment(const QString& argment)
{
    return argment;
}

QVariant TYScriptEngine::getIdNoOfArgment(TYArgment* argment)
{
    return argment->varID();
}

QVariant TYScriptEngine::getArrayIdOfArgment(const QString& argment)
{
    return argment;
}

QVariant TYScriptEngine::getEffectNoOfArgments(QList<QVariant>* argments)
{
    int count = argments->size();

    if (count == 1) {
        return getValueOfArgment(argments->at(0).toString());
    } else {
        QList<QVariant> newArgs;
        newArgs.append(QVariant());
        newArgs.append(-1);
        for (int i = 1; i < argments->size(); i++) {
            newArgs.append(argments->at(i));
        }
        m_stageManager->ty_effect(&newArgs);
        return -1;
    }
}

QVariant TYScriptEngine::intNumberWithVarID(QVariant varID)
{
    return m_scripterValues->getIntValue(varID);
}

QString TYScriptEngine::stringValueWithVarID(QVariant varID)
{
    return m_scripterValues->getStringValue(varID);
}

QVariant TYScriptEngine::intNumberWithAlias(const QString& aStr)
{
    return m_numAliases.value(aStr);
}

QString TYScriptEngine::stringValueWithAlias(const QString& aStr)
{
    return m_strAliases.value(aStr);
}

QString TYScriptEngine::stringOfReplaceVars(const QString& aString)
{
    QString result = aString;

    // Simple variable replacement - looking for $%XX patterns
    QRegExp varRegex(QString("[%$](\\d+)"));
    int pos = 0;
    while ((pos = varRegex.indexIn(result, pos)) != -1) {
        QString varType = varRegex.cap(0).left(1);
        int varIndex = varRegex.cap(1).toInt();

        QString replacement;
        if (varType == QString("$")) {
            replacement = stringValueWithVarID(varIndex);
        } else if (varType == QString("%")) {
            replacement = intNumberWithVarID(varIndex).toString();
        }

        result.replace(pos, varRegex.matchedLength(), replacement);
        pos += replacement.length();
    }

    return result;
}

void TYScriptEngine::saveExternalData()
{
    if (m_globalOn) {
        m_scripterValues->saveGlobalValues(getNScrRootDirectory() + GLOBAL_SAVEFILE_NAME);
    }

    if (m_labelLog) {
        m_labelLog->writeToFile(getNScrRootDirectory() + LABELLOG_FILENAME);
    }

    if (!m_kidokuData.isEmpty()) {
        QFile file(getNScrRootDirectory() + KIDOKU_FILENAME);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(m_kidokuData);
            file.close();
        }
    }
}

#if YYDEBUG == 1
void TYScriptEngine::ty_yydebug(QList<QVariant>* argments)
{
    if (argments->size() < 2) return;
    extern int yydebug;
    yydebug = argments->at(1).toInt();
}
#endif

void TYScriptEngine::ty_log(QList<QVariant>* argments)
{
    if (argments->size() < 2) return;
    // Simplified logging - would need arg type detection
    qDebug() << "log:" << argments->at(1).toString();
}

void TYScriptEngine::ty_globalon(QList<QVariant>* argments)
{
    Q_UNUSED(argments);
    m_globalOn = true;
}

void TYScriptEngine::ty_labellog(QList<QVariant>* argments)
{
    Q_UNUSED(argments);
    m_labelLog = new TYFileLog(getNScrRootDirectory() + LABELLOG_FILENAME, this);
}

void TYScriptEngine::ty_getversion(QList<QVariant>* argments)
{
    if (argments->size() < 2) return;
    QVariant verNum = TYEnviroment::objectForKey(QString("NScripterVersion"));
    QVariant idno = getIdNoOfArgment(nullptr);
    Q_UNUSED(verNum);
    Q_UNUSED(idno);
}

void TYScriptEngine::ty_dim(QList<QVariant>* argments)
{
    if (argments->size() < 2) return;
    QVariant idno = getIdNoOfArgment(nullptr);
    m_scripterValues->defineArrayValue(idno);
}

void TYScriptEngine::ty_movl(QList<QVariant>* argments)
{
    if (argments->size() < 3) return;
    QList<QVariant> array;
    QVariant idno = getArrayIdOfArgment(argments->at(2).toString());

    for (int i = 3; i < argments->size(); i++) {
        array.append(getValueOfArgment(argments->at(i).toString()));
    }

    m_scripterValues->setArrayLine(idno, array);
}

void TYScriptEngine::ty_mov(QList<QVariant>* argments)
{
    if (argments->size() < 3) return;
    QVariant arg = argments->at(1);
    QVariant idno = getIdNoOfArgment(nullptr);

    TYVarType type = static_cast<TYVarType>(0); // Would need proper type detection
    Q_UNUSED(type);

    // String assignment
    QString strVal = getStringOfArgment(argments->at(2).toString());
    m_scripterValues->setStringValue(idno, strVal);

    // Integer assignment (commented out for now - would need type checking)
    // QVariant intVal = getValueOfArgment(argments->at(2).toString());
    // m_scripterValues->setIntValue(idno, intVal);
}

void TYScriptEngine::ty_mov2(QList<QVariant>* argments)
{
    movRepeat(2, argments);
}

void TYScriptEngine::ty_mov3(QList<QVariant>* argments)
{
    movRepeat(3, argments);
}

void TYScriptEngine::ty_mov4(QList<QVariant>* argments)
{
    movRepeat(4, argments);
}

void TYScriptEngine::ty_mov5(QList<QVariant>* argments)
{
    movRepeat(5, argments);
}

void TYScriptEngine::ty_mov6(QList<QVariant>* argments)
{
    movRepeat(6, argments);
}

void TYScriptEngine::ty_mov7(QList<QVariant>* argments)
{
    movRepeat(7, argments);
}

void TYScriptEngine::ty_mov8(QList<QVariant>* argments)
{
    movRepeat(8, argments);
}

void TYScriptEngine::ty_mov9(QList<QVariant>* argments)
{
    movRepeat(9, argments);
}

void TYScriptEngine::ty_mov10(QList<QVariant>* argments)
{
    movRepeat(10, argments);
}

void TYScriptEngine::movRepeat(int repeat_times, QList<QVariant>* argments)
{
    int i;
    int intId;

    if (argments->size() < 3) return;

    QVariant temp = argments->at(2);
    TYVarType type = static_cast<TYVarType>(0); // Would need proper type detection
    Q_UNUSED(type);
    intId = getIdNoOfArgment(nullptr).toInt();

    if (true) { // Integer type
        for (i = 0; i < repeat_times; i++, intId++) {
            QVariant val = getValueOfArgment(argments->at(2 + i + 1).toString());
            m_scripterValues->setIntValue(intId, val.toInt());
        }
    } else {
        qDebug() << "movX, error of arg 1, invalid context";
    }
}

void TYScriptEngine::ty_intlimit(QList<QVariant>* argments)
{
    if (argments->size() < 4) return;
    QVariant id = getValueOfArgment(argments->at(1).toString());
    QVariant low = getValueOfArgment(argments->at(2).toString());
    QVariant high = getValueOfArgment(argments->at(3).toString());
    m_scripterValues->setIntLimitID(id, low.toInt(), high.toInt());
}

void TYScriptEngine::ty_inc(QList<QVariant>* argments)
{
    if (argments->size() < 2) return;
    QVariant idno = getIdNoOfArgment(nullptr);
    int number = m_scripterValues->getIntValue(idno).toInt();
    number++;
    m_scripterValues->setIntValue(idno, number);
}

void TYScriptEngine::ty_dec(QList<QVariant>* argments)
{
    if (argments->size() < 2) return;
    QVariant idno = getIdNoOfArgment(nullptr);
    int number = m_scripterValues->getIntValue(idno).toInt();
    number--;
    m_scripterValues->setIntValue(idno, number);
}

void TYScriptEngine::ty_add(QList<QVariant>* argments)
{
    if (argments->size() < 3) return;
    QVariant idno = getIdNoOfArgment(nullptr);
    
    QString argstr = getStringOfArgment(argments->at(2).toString());
    QString string = m_scripterValues->getStringValue(idno);
    string = string + argstr;
    m_scripterValues->setStringValue(idno, string);
}

void TYScriptEngine::ty_sub(QList<QVariant>* argments)
{
    if (argments->size() < 3) return;
    QVariant idno = getIdNoOfArgment(nullptr);
    QVariant argnum = getValueOfArgment(argments->at(2).toString());
    int number = m_scripterValues->getIntValue(idno).toInt();
    number -= argnum.toInt();
    m_scripterValues->setIntValue(idno, number);
}

void TYScriptEngine::ty_mul(QList<QVariant>* argments)
{
    if (argments->size() < 3) return;
    QVariant idno = getIdNoOfArgment(nullptr);
    QVariant argnum = getValueOfArgment(argments->at(2).toString());
    int number = m_scripterValues->getIntValue(idno).toInt();
    number *= argnum.toInt();
    m_scripterValues->setIntValue(idno, number);
}

void TYScriptEngine::ty_div(QList<QVariant>* argments)
{
    if (argments->size() < 3) return;
    QVariant idno = getIdNoOfArgment(nullptr);
    QVariant argnum = getValueOfArgment(argments->at(2).toString());
    int number = m_scripterValues->getIntValue(idno).toInt();
    if (argnum.toInt() != 0) {
        number /= argnum.toInt();
    }
    m_scripterValues->setIntValue(idno, number);
}

void TYScriptEngine::ty_mod(QList<QVariant>* argments)
{
    if (argments->size() < 3) return;
    QVariant idno = getIdNoOfArgment(nullptr);
    QVariant argnum = getValueOfArgment(argments->at(2).toString());
    int number = m_scripterValues->getIntValue(idno).toInt();
    if (argnum.toInt() != 0) {
        number %= argnum.toInt();
    }
    m_scripterValues->setIntValue(idno, number);
}

void TYScriptEngine::ty_cmp(QList<QVariant>* argments)
{
    if (argments->size() < 4) return;
    QVariant idno = getIdNoOfArgment(nullptr);
    QString str1 = getStringOfArgment(argments->at(2).toString());
    QString str2 = getStringOfArgment(argments->at(3).toString());
    int result = QString::compare(str1, str2);
    m_scripterValues->setIntValue(idno, result);
}

void TYScriptEngine::ty_atoi(QList<QVariant>* argments)
{
    if (argments->size() < 3) return;
    QVariant idno = getIdNoOfArgment(nullptr);
    QString str = getStringOfArgment(argments->at(2).toString());
    m_scripterValues->setIntValue(idno, str.toInt());
}

void TYScriptEngine::ty_itoa(QList<QVariant>* argments)
{
    if (argments->size() < 3) return;
    QVariant idno = getIdNoOfArgment(nullptr);
    QVariant value = getValueOfArgment(argments->at(2).toString());
    m_scripterValues->setStringValue(idno, QString::number(value.toInt()));
}

void TYScriptEngine::ty_len(QList<QVariant>* argments)
{
    if (argments->size() < 3) return;
    QVariant idno = getIdNoOfArgment(nullptr);
    QString str = getStringOfArgment(argments->at(2).toString());
    m_scripterValues->setIntValue(idno, str.length());
}

void TYScriptEngine::ty_mid(QList<QVariant>* argments)
{
    if (argments->size() < 5) return;
    QVariant idno = getIdNoOfArgment(nullptr);
    QString inStr = getStringOfArgment(argments->at(2).toString());
    int offset = getValueOfArgment(argments->at(3).toString()).toInt();
    int length = getValueOfArgment(argments->at(4).toString()).toInt();
    QString result = inStr.mid(offset, length);
    m_scripterValues->setStringValue(idno, result);
}

void TYScriptEngine::ty_splitstring(QList<QVariant>* argments)
{
    if (argments->size() < 4) return;
    QString scrStr = argments->at(1).toString();
    QString tmpStr;
    QStringList parts = scrStr.split(argments->at(2).toString());

    for (int i = 0; i < parts.size() && (i + 3) < argments->size(); i++) {
        QList<QVariant> movArgs;
        movArgs.append(QString("mov"));
        movArgs.append(argments->at(3 + i));
        movArgs.append(parts.at(i));
        ty_mov(&movArgs);
    }
}

void TYScriptEngine::ty_rnd(QList<QVariant>* argments)
{
    if (argments->size() < 3) return;
    QVariant idno = getIdNoOfArgment(nullptr);
    int max = getValueOfArgment(argments->at(2).toString()).toInt();
    int result = TYRandom(0, max - 1);
    m_scripterValues->setIntValue(idno, result);
}

void TYScriptEngine::ty_rnd2(QList<QVariant>* argments)
{
    if (argments->size() < 4) return;
    QVariant idno = getIdNoOfArgment(nullptr);
    int min = getValueOfArgment(argments->at(2).toString()).toInt();
    int max = getValueOfArgment(argments->at(3).toString()).toInt();
    int result = TYRandom(min, max);
    m_scripterValues->setIntValue(idno, result);
}

void TYScriptEngine::ty_date(QList<QVariant>* argments)
{
    QDateTime cDate = QDateTime::currentDateTime();
    QDate date = cDate.date();

    if (argments->size() >= 2) {
        QVariant idno = getIdNoOfArgment(nullptr);
        m_scripterValues->setIntValue(idno, date.year());
    }
    if (argments->size() >= 3) {
        QVariant idno = getIdNoOfArgment(nullptr);
        m_scripterValues->setIntValue(idno, date.month());
    }
    if (argments->size() >= 4) {
        QVariant idno = getIdNoOfArgment(nullptr);
        m_scripterValues->setIntValue(idno, date.day());
    }
}

void TYScriptEngine::ty_time(QList<QVariant>* argments)
{
    QDateTime cDate = QDateTime::currentDateTime();
    QTime time = cDate.time();

    if (argments->size() >= 2) {
        QVariant idno = getIdNoOfArgment(nullptr);
        m_scripterValues->setIntValue(idno, time.hour());
    }
    if (argments->size() >= 3) {
        QVariant idno = getIdNoOfArgment(nullptr);
        m_scripterValues->setIntValue(idno, time.minute());
    }
    if (argments->size() >= 4) {
        QVariant idno = getIdNoOfArgment(nullptr);
        m_scripterValues->setIntValue(idno, time.second());
    }
}

void TYScriptEngine::ty_stralias(QList<QVariant>* argments)
{
    if (argments->size() < 3) return;
    QString key = argments->at(1).toString().toLower();
    QString value = getStringOfArgment(argments->at(2).toString());
    m_strAliases[key] = value;
}

void TYScriptEngine::ty_numalias(QList<QVariant>* argments)
{
    if (argments->size() < 3) return;
    QString key = argments->at(1).toString().toLower();
    QVariant value = getValueOfArgment(argments->at(2).toString());
    m_numAliases[key] = value;
}

void TYScriptEngine::ty_arc(QList<QVariant>* argments)
{
    if (argments->size() < 2) return;
    TYResourceServer::sharedServer()->addArchiver(getStringOfArgment(argments->at(1).toString()));
}

void TYScriptEngine::ty_nsa(QList<QVariant>* argments)
{
    Q_UNUSED(argments);
    QString dir = m_nsaDir.isNull() ? QString("") : m_nsaDir;

    TYResourceServer::sharedServer()->addArchiver(dir + DEFALUT_NSA_ARCHIVE.arg(QString("")));

    for (int i = 1; i <= 9; i++) {
        TYResourceServer::sharedServer()->addArchiver(dir + DEFALUT_NSA_ARCHIVE.arg(QString::number(i)));
    }
}

void TYScriptEngine::ty_nsadir(QList<QVariant>* argments)
{
    if (argments->size() < 2) return;
    m_nsaDir = getStringOfArgment(argments->at(1).toString());
}

void TYScriptEngine::ty_ns2(QList<QVariant>* argments)
{
    Q_UNUSED(argments);
    TYNsaArchiver::setMode(TY2TypeNsaFileMode);
    ty_nsa(nullptr);
}

void TYScriptEngine::ty_ns3(QList<QVariant>* argments)
{
    Q_UNUSED(argments);
    TYNsaArchiver::setMode(TY3TypeNsaFileMode);
    ty_nsa(nullptr);
}

void TYScriptEngine::ty_jumpf(QList<QVariant>* argments)
{
    Q_UNUSED(argments);
    labeljump(QString("~"), m_runLine, false);
}

void TYScriptEngine::ty_jumpb(QList<QVariant>* argments)
{
    Q_UNUSED(argments);
    labeljump(QString("~"), m_runLine, true);
}

void TYScriptEngine::ty_goto(QList<QVariant>* argments)
{
    if (argments->size() < 2) return;
    labeljump(getStringOfArgment(argments->at(1).toString()));
}

void TYScriptEngine::ty_gosub(QList<QVariant>* argments)
{
    if (argments->size() < 2) return;
    TYScriptPoint sPoint;
    sPoint = TYMakeScriptPoint(m_runLine, m_runColumn);
    m_returnStack.append(QVariant::fromValue(sPoint));
    labeljump(getStringOfArgment(argments->at(1).toString()));
}

void TYScriptEngine::ty_return(QList<QVariant>* argments)
{
    Q_UNUSED(argments);
    if (m_returnStack.isEmpty()) return;
    
    TYScriptPoint sPoint = m_returnStack.last().value<TYScriptPoint>();
    jump(sPoint.line);
    m_runColumn = sPoint.column;
    m_returnStack.removeLast();

    if ((m_textgosubPointIndex > -1) && (m_textgosubPointIndex == m_returnStack.size())) {
        setBreakRun(true);
        if (m_stageManager) {
            m_stageManager->resumePrinting();
        }
        m_textgosubPointIndex = -1;
    }
}

void TYScriptEngine::ty_tablegoto(QList<QVariant>* argments)
{
    if (argments->size() < 3) return;
    int num = getValueOfArgment(argments->at(1).toString()).toInt();
    QList<QVariant> gotoArgs;
    gotoArgs.append(QString("goto"));
    gotoArgs.append(argments->at(2 + num));
    ty_goto(&gotoArgs);
}

void TYScriptEngine::ty_skip(QList<QVariant>* argments)
{
    if (argments->size() < 2) return;
    int step = getValueOfArgment(argments->at(1).toString()).toInt();
    if (step) {
        m_runLine += step - 1;
    }
}

void TYScriptEngine::ty_next(QList<QVariant>* argments)
{
    Q_UNUSED(argments);
    TYLoopStruct loop;

    if (m_loopStack.isEmpty()) {
        qDebug() << "do next, but no loop stack";
        return;
    }

    TYLoopValue* loopValue = m_loopStack.last().value<TYLoopValue*>();
    if (!loopValue) return;
    loop = loopValue->loopStruct();

    int now = m_scripterValues->getIntValue(loop.varNo).toInt() + loop.step;
    m_scripterValues->setIntValue(loop.varNo, now);

    if ((loop.step > 0 && now > loop.limit) || (loop.step < 0 && now < loop.limit)) {
        qDebug() << "remove for stack";
        m_loopStack.removeLast();
    } else {
        m_runLine = loop.point.line;
        m_runColumn = loop.point.column;
        if (m_runColumn == 0) {
            m_runColumn++;
        }
    }
}

void TYScriptEngine::ty_break(QList<QVariant>* argments)
{
    qDebug() << "remove for stack";
    if (!m_loopStack.isEmpty()) {
        m_loopStack.removeLast();
    }
    if (argments->size() >= 2) {
        ty_goto(argments);
    } else {
        jumpToLoopEnd();
    }
}

void TYScriptEngine::ty_game(QList<QVariant>* argments)
{
    Q_UNUSED(argments);
    labeljump(QString("*start"));
}

void TYScriptEngine::ty_resettimer(QList<QVariant>* argments)
{
    Q_UNUSED(argments);
    m_timerStartDate = QDateTime::currentDateTime();
}

void TYScriptEngine::ty_waittimer(QList<QVariant>* argments)
{
    if (argments->size() < 2) return;
    int timerValue = getValueOfArgment(argments->at(1).toString()).toInt();
    setBreakRun(true);
    // In Qt, would use QTimer for delayed execution
}

void TYScriptEngine::ty_gettimer(QList<QVariant>* argments)
{
    if (argments->size() < 2) return;
    QVariant idno = getIdNoOfArgment(nullptr);
    qint64 msecs = m_timerStartDate.msecsTo(QDateTime::currentDateTime());
    m_scripterValues->setIntValue(idno, -msecs);
}

void TYScriptEngine::ty_loadgame(QList<QVariant>* argments)
{
    if (argments->size() < 2) return;
    QVariant number = getValueOfArgment(argments->at(1).toString());
    if (m_controller) {
        m_controller->loadLocalData(number);
    }
}

void TYScriptEngine::ty_savegame(QList<QVariant>* argments)
{
    if (argments->size() < 2) return;
    QVariant number = getValueOfArgment(argments->at(1).toString());
    if (m_controller) {
        m_controller->saveLocalData(number);
    }
}

void TYScriptEngine::ty_end(QList<QVariant>* argments)
{
    Q_UNUSED(argments);
    // In Qt, would use QCoreApplication::quit()
    qApp->quit();
}

void TYScriptEngine::ty_inputstr(QList<QVariant>* argments)
{
    if (argments->size() < 5) return;
    TYInputStrController* input = TYInputStrController::dialog();
    QString caption = getStringOfArgment(argments->at(2).toString());
    int length = getValueOfArgment(argments->at(3).toString()).toInt();
    int notAscii = getValueOfArgment(argments->at(4).toString()).toInt();

    QVariant idno = getIdNoOfArgment(nullptr);
    QString result = input->runModalCaption(caption, length, notAscii);
    m_scripterValues->setStringValue(idno, result);
}

void TYScriptEngine::ty_input(QList<QVariant>* argments)
{
    if (argments->size() < 6) return;
    TYInputStrController* input = TYInputStrController::dialog();
    QString caption = getStringOfArgment(argments->at(2).toString());
    QString defaultStr = getStringOfArgment(argments->at(3).toString());
    int length = getValueOfArgment(argments->at(4).toString()).toInt();
    int notAscii = getValueOfArgment(argments->at(5).toString()).toInt();

    QVariant idno = getIdNoOfArgment(nullptr);
    QString result = input->runModalCaption(caption, length, notAscii);
    m_scripterValues->setStringValue(idno, result);
}

void TYScriptEngine::ty_mesbox(QList<QVariant>* argments)
{
    if (argments->size() < 3) return;
    QString message = getStringOfArgment(argments->at(1).toString());
    QString title = getStringOfArgment(argments->at(2).toString());
    QMessageBox::information(nullptr, title, message);
}

void TYScriptEngine::ty_getreg(QList<QVariant>* argments)
{
    if (argments->size() < 4) return;
    QString regFile = getNScrRootDirectory() + REGISTRY_NAME;
    QFile file(regFile);
    QVariantMap aDict;

    if (file.open(QIODevice::ReadOnly)) {
        QDataStream stream(&file);
        stream >> aDict;
        file.close();
    }

    QString key = getStringOfArgment(argments->at(2).toString());
    QString name = getStringOfArgment(argments->at(3).toString());

    if (!aDict.isEmpty()) {
        QVariantMap keyDict = aDict.value(key).toMap();
        QString value = keyDict.value(name).toString();
        QVariant idno = getIdNoOfArgment(nullptr);
        m_scripterValues->setStringValue(idno, value);
    }
}

void TYScriptEngine::ty_getcursorpos(QList<QVariant>* argments)
{
    if (argments->size() < 3) return;
    QVariant varX = getIdNoOfArgment(nullptr);
    QVariant varY = getIdNoOfArgment(nullptr);

    QPoint point = m_stageManager->drawPoint();
    m_scripterValues->setIntValue(varX, static_cast<int>(point.x()));
    m_scripterValues->setIntValue(varY, VSCREEN_HEIGHT - static_cast<int>(point.y()));
}

void TYScriptEngine::ty_lr_trap(QList<QVariant>* argments)
{
    ty_trap(argments);
}

void TYScriptEngine::ty_trap(QList<QVariant>* argments)
{
    if (argments->size() < 2) return;
    QString arg = argments->at(1).toString();
    QString carg = getStringOfArgment(arg);

    if (arg.toLower() == QString("off")) {
        m_labelWithTrap.clear();
        m_fireOfTrap = false;
        if (m_controller) {
            m_controller->setTrap(false);
        }
    } else if (arg.toLower() == QString("stop")) {
        m_fireOfTrap = false;
        if (m_controller) {
            m_controller->setTrap(false);
        }
    } else if (arg.toLower() == QString("resume")) {
        m_fireOfTrap = false;
        if (m_controller) {
            m_controller->setTrap(true);
        }
    } else {
        m_labelWithTrap = carg;
        m_fireOfTrap = false;
        if (m_controller) {
            m_controller->setTrap(true);
        }
    }
}

void TYScriptEngine::fireOfTrap()
{
    if (!m_labelWithTrap.isNull()) {
        m_fireOfTrap = true;
    }
}

void TYScriptEngine::fireOfTextgosub(const QString& aLabel)
{
    TYScriptPoint point;
    point = TYMakeScriptPoint(m_runLine, m_runColumn);
    QList<QVariant> gosubArgs;
    gosubArgs.append(QString("gosub"));
    gosubArgs.append(aLabel);
    ty_gosub(&gosubArgs);
    m_textgosubPointIndex = m_returnStack.size() - 1;
    runScript();
}

void TYScriptEngine::enableKidoku()
{
    QString filename = getNScrRootDirectory() + KIDOKU_FILENAME;
    QFile file(filename);
    
    if (file.open(QIODevice::ReadOnly)) {
        m_kidokuData = file.readAll();
        file.close();
    }

    if (m_kidokuData.isEmpty()) {
        int size = m_scriptArray.size() / 8 + 1;
        m_kidokuData.resize(size);
        m_kidokuData.fill(0);
    } else {
        if (m_kidokuData.size() < m_scriptArray.size() / 8 + 1) {
            m_kidokuData.resize(m_scriptArray.size() / 8 + 1);
        }
    }

    m_kidokuptr = (unsigned char*)m_kidokuData.data();
}

void TYScriptEngine::setWatchKidoku(bool aBool)
{
    m_isWatchKidoku = aBool;
}
