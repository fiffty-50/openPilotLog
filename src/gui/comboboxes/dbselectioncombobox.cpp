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
    DEB << "Refreshing model...";
    m_map.clear();

    // Get the data from the DB and fill the map
    QSqlQuery q(getQuery(m_completionTarget));
    q.exec();
    while (q.next()) {
        m_map.insert(q.value(1).toString(), q.value(0).toInt());
    }

    // Save state, then re-fill the combobox (add rowid to data)
    QSignalBlocker b(this);
    QString current = currentText();

    clear();
    for (auto it = m_map.constBegin(); it != m_map.constEnd(); ++it) {
        addItem(it.key(), it.value());
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
}

void DbSelectionComboBox::on_editingFinished()
{
    const auto &text = lineEdit()->text();

    if (text.isEmpty()) return;
    if (m_map.contains(text)) return;

    if (completionIsAvailable()) {
        setCurrentText(completer()->currentCompletion());
    }
    else {
        emit newValueEntered(this);
    }
}

bool DbSelectionComboBox::completionIsAvailable()
{
    if (!completer()) return false;
    completer()->setCompletionPrefix(currentText());
    return completer()->completionCount() > 0;
}
