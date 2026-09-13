//
//  TYArgmentArray.m
//  Tukuyomi
//
//  Created by toveta on Wed Oct 30 2002.
//  Copyright (c) 2002 toveta All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY 
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 * &ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "TYArgmentArray.h"
#include "TYScriptEngine.h"


#define MAX_LINE_LENGTH 4096
// gnscripterからコピペ(ぉ
#define myisdigit(c) ((c) >= '0' && (c) <= '9')
#define myishexdigit(c) (myisdigit(c) || (c) >= 'a' && (c) <= 'f' || (c) >= 'A' && (c) <= 'F')
#define myisalpha(c) ((c) >= 'a' && (c) <= 'z' || (c) >= 'A' && (c) <= 'Z' || (c) == '_')
#define myisalnum(c) (myisdigit(c) || myisalpha(c))


enum class TYArgContext {
    NoArg = -1,
    Undefined,
    Operand,
    String,
    Integer,
    Separater,
    Chain
};

static TYArgContext get_item(const char** ptr, char* buffer)
{
    TYArgContext context = TYArgContext::Undefined;

    if (**ptr == '%') {
        context = TYArgContext::Integer;
        // get_vararg(ptr, buffer);
        return context;
    } else if (**ptr == '$') {
        context = TYArgContext::String;
        // get_vararg(ptr, buffer);
        return context;
    } else if (**ptr == '"') {
        context = TYArgContext::String;
        *buffer = **ptr; buffer++; (*ptr)++;
        while (**ptr && **ptr != '"') {
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        *buffer = **ptr; buffer++; (*ptr)++;
        *buffer = '\0';
        return context;
    } else if (**ptr == '(') {
        char strbuffer[MAX_LINE_LENGTH + 1];
        // MEMO (string) string string 構文に対応
        context = TYArgContext::String;

        *buffer = **ptr;
        (*ptr)++; buffer++;
        while (**ptr && isspace(**ptr)) {
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        // get_item(ptr, strbuffer);
        *buffer = '\0';
        // strcat(buffer, strbuffer);
        buffer += strlen(strbuffer);
        while (**ptr && isspace(**ptr)) {
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        *buffer = **ptr;
        (*ptr)++; buffer++;
        // second
        while (**ptr && isspace(**ptr)) {
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        // get_item(ptr, strbuffer);
        *(buffer) = '\0';
        // strcat(strbuffer, " ");
        // strcat(buffer, strbuffer);
        buffer += strlen(strbuffer);
        // third
        while (**ptr && isspace(**ptr)) {
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        // get_item(ptr, strbuffer);
        *(buffer) = '\0';
        // strcat(buffer, strbuffer);

        return context;
    } else if (isdigit(**ptr) || (**ptr == '-')) {
        context = TYArgContext::Integer;
        *buffer = **ptr;
        (*ptr)++; buffer++;
        while (**ptr && isdigit(**ptr)) {
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        *buffer = '\0';
        return context;
    } else if (isalpha(**ptr)) {
        context = TYArgContext::Undefined;
        while (**ptr && myisalnum(**ptr)) {
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        *buffer = '\0';
        return context;
    } else if (**ptr && strchr("!<>=", **ptr)) {
        context = TYArgContext::Operand;
        while (**ptr && strchr("!<>=", **ptr)) {
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        *buffer = '\0';
        return context;
    } else if (**ptr == ',') {
        context = TYArgContext::Separater;
        *buffer = **ptr;
        (*ptr)++; buffer++;
        *buffer = '\0';
        return context;
    } else if (**ptr == '&') {
        context = TYArgContext::Chain;
        while (**ptr && **ptr == '&') {
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        *buffer = '\0';
        return context;
    } else if (**ptr == '*' || **ptr == '#') {
        context = TYArgContext::String;
        *buffer = **ptr;
        (*ptr)++; buffer++;
        while (**ptr && myisalnum(**ptr)) {
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        *buffer = '\0';
        return context;
    }

    return TYArgContext::NoArg;
}

static void space_skip(const char** ptr)
{
    while (**ptr && isspace(**ptr))
        (*ptr)++;
}

TYArgmentArray::TYArgmentArray(const char* aCStr, const char* cmdCStr, QObject* parent)
    : QObject(parent)
    , m_isParsed(false)
    , m_parseMode(TYParseMode::Normal)
{
    m_sourceData = QString::fromLatin1(aCStr);
    m_parsedArgments.append(QString::fromLatin1(cmdCStr));
}

TYArgmentArray* TYArgmentArray::arrayWithCString(const char* aCStr, const char* cmdCStr)
{
    return new TYArgmentArray(aCStr, cmdCStr);
}

// パースモードは外から指定する
void TYArgmentArray::setParseMode(TYParseMode mode)
{
    m_parseMode = mode;
}

void TYArgmentArray::parse()
{
    m_isParsed = true;

    switch (m_parseMode) {
    case TYParseMode::Normal:
    case TYParseMode::Condition:
    case TYParseMode::Loop:
        break;
    }
}

QVariant TYArgmentArray::at(int aIndex)
{
    if ((aIndex != 0) && (!m_isParsed)) {
        parse();
    }
    return QVariant(m_parsedArgments[aIndex]);
}

int TYArgmentArray::count() const
{
    return m_parsedArgments.size();
}
