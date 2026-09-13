//
//  TYSaveLoadDialogController.h
//  Tukuyomi
//
//  Created by toveta on Sun Jan 06 2002.
//  Copyright (c) 2001 toveta All rights reserved.
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

#ifndef TYSaveLoadDialogController_H
#define TYSaveLoadDialogController_H

#include <QObject>
#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QMap>
#include <QDateTime>

#define DIALOG_OK 128
#define DIALOG_CANCEL 129

// TableViewのデータソース、ダイアログのController、セーブデータの管理オブジェクト、の３つの役割を持つ。
class TYSaveLoadDialogController : public QObject {
    Q_OBJECT

public:
    TYSaveLoadDialogController(QObject* parent = nullptr);
    ~TYSaveLoadDialogController();

public slots:
    void dialogOk();
    void dialogCancel();
    void doubleClick();
    void onTableSelectionChanged();

public:
    QDialog* window();
    void setTitle(const QString& title);
    void setNumber(int number);
    int number() const;
    void setDate(const QDateTime& aDate, int number);
    void setLoaded(bool aBool);
    bool isLoaded() const;
    void setLoadMode(bool aBool);
    int selectedRow() const;

    int rowCount(const QTableWidget* tableView) const;
    QString tableView(const QTableWidget* tableView, const QString& columnId, int row) const;

private:
    QDialog* m_window;
    QTableWidget* tableView;
    QPushButton* okButton;

    QMap<int, QDateTime> dataDict;
    int dataNum;
    int selectedRowIndex;
    bool loaded;
    bool loadMode;
};

#endif // TYSaveLoadDialogController_H
