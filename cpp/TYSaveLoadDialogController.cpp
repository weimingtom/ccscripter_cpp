//
//  TYSaveLoadDialogController.m
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

#include "TYSaveLoadDialogController.h"
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QTableWidgetItem>
#include <QHeaderView>

TYSaveLoadDialogController::TYSaveLoadDialogController(QObject* parent)
    : QObject(parent)
    , m_window(nullptr)
    , tableView(nullptr)
    , okButton(nullptr)
    , dataNum(9)
    , selectedRowIndex(-1)
    , loaded(false)
    , loadMode(false)
{
    m_window = new QDialog();
    m_window->setWindowTitle(QString::fromLocal8Bit("Save/Load"));

    tableView = new QTableWidget();
    tableView->setColumnCount(2);
    tableView->setHorizontalHeaderLabels(QStringList() << QString::fromLocal8Bit("No") << QString::fromLocal8Bit("Date"));
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    okButton = new QPushButton(QString::fromLocal8Bit("OK"));
    okButton->setEnabled(false);

    QDialogButtonBox* buttons = new QDialogButtonBox();
    buttons->addButton(okButton, QDialogButtonBox::AcceptRole);
    buttons->addButton(QString::fromLocal8Bit("Cancel"), QDialogButtonBox::RejectRole);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addWidget(tableView);
    mainLayout->addWidget(buttons);
    m_window->setLayout(mainLayout);

    connect(buttons, &QDialogButtonBox::accepted, this, &TYSaveLoadDialogController::dialogOk);
    connect(buttons, &QDialogButtonBox::rejected, this, &TYSaveLoadDialogController::dialogCancel);
    connect(tableView, &QTableWidget::itemDoubleClicked, this, &TYSaveLoadDialogController::doubleClick);
    connect(tableView, &QTableWidget::itemSelectionChanged, this, &TYSaveLoadDialogController::onTableSelectionChanged);
}

TYSaveLoadDialogController::~TYSaveLoadDialogController()
{
    if (m_window) {
        delete m_window;
    }
}

void TYSaveLoadDialogController::dialogOk()
{
    selectedRowIndex = tableView->currentRow();
    m_window->done(DIALOG_OK);
}

void TYSaveLoadDialogController::dialogCancel()
{
    m_window->done(DIALOG_CANCEL);
}

void TYSaveLoadDialogController::doubleClick()
{
    if (okButton->isEnabled()) {
        dialogOk();
    }
}

void TYSaveLoadDialogController::onTableSelectionChanged()
{
    if (!loadMode) {
        okButton->setEnabled(true);
        return;
    }

    int row = tableView->currentRow();
    if (dataDict.contains(row + 1)) {
        okButton->setEnabled(true);
    } else {
        okButton->setEnabled(false);
    }
}

QDialog* TYSaveLoadDialogController::window()
{
    return m_window;
}

void TYSaveLoadDialogController::setTitle(const QString& title)
{
    m_window->setWindowTitle(title);
}

void TYSaveLoadDialogController::setNumber(int number)
{
    dataNum = number;
    tableView->setRowCount(dataNum);
    for (int i = 0; i < dataNum; ++i) {
        tableView->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
    }
}

int TYSaveLoadDialogController::number() const
{
    return dataNum;
}

void TYSaveLoadDialogController::setDate(const QDateTime& aDate, int number)
{
    dataDict[number] = aDate;
    if (number >= 1 && number <= dataNum) {
        tableView->setItem(number - 1, 1, new QTableWidgetItem(aDate.toString(QString::fromLocal8Bit("yyyy/MM/dd HH:mm"))));
    }
}

void TYSaveLoadDialogController::setLoaded(bool aBool)
{
    loaded = aBool;
}

bool TYSaveLoadDialogController::isLoaded() const
{
    return loaded;
}

int TYSaveLoadDialogController::selectedRow() const
{
    return selectedRowIndex;
}

void TYSaveLoadDialogController::setLoadMode(bool aBool)
{
    loadMode = aBool;
}
