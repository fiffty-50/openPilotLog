/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2026 Felix Turowsky
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "dbselectioncombobox.h"
#include "src/gui/verification/diacriticignoringcompleter.h"
#include "src/opl.h"
#include <QCompleter>
#include <QObject>
#include <QSqlQuery>

DbSelectionComboBox::DbSelectionComboBox(OPL::DbTable table, QWidget *parent)
    : QComboBox(parent), m_table(table)
{
    setEditable(true);
    connectSlots();
}

void DbSelectionComboBox::refresh()
{
    LOG << "Updating combo box for table: " << OPL::GLOBALS->getDbTableName(m_table);
    this->blockSignals(true);
    this->lineEdit()->blockSignals(true);

    // clear and re-set the box
    clear();
    auto map = getMap();
    for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
        addItem(it.key(), it.value());
    }

    // re-set the completer
    setCompleter(nullptr);
    auto completer = DiacriticIgnoringCompleter::createCompleter(map.keys(), this);

    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setFilterMode(Qt::MatchContains);
    setCompleter(completer);
    this->blockSignals(false);
    this->lineEdit()->blockSignals(false);
}

bool DbSelectionComboBox::verifyContent()
{
    const auto &text = currentText();
    if(text.isEmpty()) return false;

    if(getMap().contains(text)) {
        setStyleSheet({});
        return true;
    } else {
        setStyleSheet(OPL::CssStyles::RED_BORDER);
        return false;
    }
}

void DbSelectionComboBox::connectSlots()
{

    // trigger validation when content is changed by user or programatically
    connect(this, &QComboBox::activated, this, [this]() { verifyContent(); });
    connect(this, &QComboBox::highlighted, this, [this](int idx) { setCurrentIndex(idx); });
    connect(this, &QComboBox::currentIndexChanged, this, [this]() { verifyContent(); });

    // When the widget loses focus while the user has been typing, try to use an autocompletion.
    // If unable, emit newValueEntered
    connect(lineEdit(), &QLineEdit::editingFinished, this,
            &DbSelectionComboBox::on_editingFinished);
}

void DbSelectionComboBox::on_editingFinished()
{
    {
        // Block Signals while trying to fix the input using the completer
        QSignalBlocker b(lineEdit());
        QSignalBlocker c(this);
        const auto &text = lineEdit()->text();
        if (text.isEmpty()) return;

        // if the input is contained in the map, it is valid
        if (getMap().contains(text)) {
            setStyleSheet(QString());
            return;
        }

        // try to use a completion
        if (completionIsAvailable()) {
            setCurrentText(completer()->currentCompletion());
            setStyleSheet({});
            return;
        }
        else {
            setStyleSheet(OPL::CssStyles::RED_BORDER);
        }

    } // Signal blockers go out of scope
    emit newValueEntered(this);
}

bool DbSelectionComboBox::completionIsAvailable()
{
    if (!completer()) return false;
    completer()->setCompletionPrefix(currentText());
    return completer()->completionCount() > 0;
}
