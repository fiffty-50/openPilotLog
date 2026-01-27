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
#include "src/database/database.h"
#include "src/gui/verification/diacriticignoringcompleter.h"
#include "src/opl.h"
#include <qassert.h>
#include <qcompleter.h>
#include <qobject.h>

DbSelectionComboBox::DbSelectionComboBox(CompletionTarget target, QWidget *parent)
    : QComboBox(parent), m_completionTarget(target)
{
    setEditable(true);
    refresh();
    connectSlots();
}

void DbSelectionComboBox::refresh()
{

    // Get the data from the DB and fill the map
    m_map.clear();
    QSqlQuery q(getQuery(m_completionTarget));
    q.exec();
    int i = 0;
    while (q.next()) {
        m_map.insert(q.value(1).toString(), q.value(0).toInt());
        if(i < 5 && m_completionTarget == AirportCodes) {
            DEB << "Adding to map: " << q.value(1).toString() << " id: " << q.value(0).toInt();
            i++;
        }
    }

    // Save state, then re-fill the combobox (add rowid to data)
    QSignalBlocker b(this);
    QString current = currentText();

    clear();
    i = 0;
    for (auto it = m_map.constBegin(); it != m_map.constEnd(); ++it) {
        addItem(it.key(), it.value());
        if(it.key() == "AYGA" || it.key() == "AYMD")
            DEB << "Adding to box: " << it.key() << " with data: " << it.value();
    }

    setCurrentText(current);

    // re-set the completer
    setCompleter(nullptr);
    auto completer = DiacriticIgnoringCompleter::createCompleter(m_map.keys(), this);

    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setFilterMode(Qt::MatchContains);
    setCompleter(completer);
}

void DbSelectionComboBox::connectSlots()
{
    // When the widget loses focus while the user has been typing, try to use an autocompletion.
    // If unable, emit newValueEntered
    connect(lineEdit(), &QLineEdit::editingFinished, this,
            &DbSelectionComboBox::on_editingFinished);

    // Update the model and completer when the database changes
    connect(DB, &OPL::Database::dataBaseUpdated, this, &DbSelectionComboBox::refresh);

    // Make sure that Completion clears the style Sheet
    connect(this, &QComboBox::activated, this, [this]() {
        DEB << "CB activated...";
        setStyleSheet(QString());
    });
    connect(this, &QComboBox::highlighted, this, [this](int idx) { setCurrentIndex(idx); });
}

void DbSelectionComboBox::on_editingFinished()
{
    const auto &text = lineEdit()->text();

    if (text.isEmpty()) return;
    if (m_map.contains(text)) {
        setStyleSheet(QString());
        return;
    }

    if (completionIsAvailable()) {
        setCurrentText(completer()->currentCompletion());
    }
    else {
        setStyleSheet(OPL::CssStyles::RED_BORDER);
        emit newValueEntered(this);
    }
}

bool DbSelectionComboBox::completionIsAvailable()
{
    if (!completer()) return false;
    completer()->setCompletionPrefix(currentText());
    return completer()->completionCount() > 0;
}
