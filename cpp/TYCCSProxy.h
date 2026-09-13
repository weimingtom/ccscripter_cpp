//
//  TYCCSProxy.h
//  Tukuyomi
//
//  Created by toveta on Sun Aug 03 2003.
//  Copyright (c) 2003 toveta. All rights reserved.
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

#ifndef TYCCSProxy_h
#define TYCCSProxy_h

#include <QObject>
#include <QString>
#include "TYCCScripterProxy.h"

class TYCCSProxy : public QObject, public TYCCScripterProxy {
    Q_OBJECT

public:
    static TYCCSProxy* proxy();

    explicit TYCCSProxy(QObject* parent = nullptr);
    ~TYCCSProxy();

    // TYCCScripterProxy interface
    QObject* mainWindow() override;
    void setReturnCode(int aInt, const QString& aStr) override;
    int returnCode() const override { return m_returnCode; }
    QString returnString() const override { return m_returnString; }

private:
    static TYCCSProxy* sharedProxy;

    int m_returnCode;
    QString m_returnString;
};

#endif // TYCCSProxy_h
