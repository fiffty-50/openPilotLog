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
#ifndef FLIGHTLOGENTRYEDITDIALOG_H
#define FLIGHTLOGENTRYEDITDIALOG_H

#include "entryeditdialog.h"
#include "src/gui/comboboxes/dbselectioncombobox.h"
#include "src/gui/dialogues/flightentryeditui.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTimeEdit>

namespace OPL {

class FlightLogEntryEditDialog : public EntryEditDialog {
  public:
    explicit FlightLogEntryEditDialog(QWidget *parent = nullptr);
    ~FlightLogEntryEditDialog() { delete ui; }

    // Entry Edit Interface
    void loadEntry(int rowID) override;
    bool deleteEntry(int rowID) override;
    void reset() override;

  private:
    // Ui
    Ui::FlightEntryEditUi *ui;
    // dialog setup
    void init();
    // void retranslateUi();
    void setupValidationAndCompletion();
    void setupSlots();
    void readSettings();

    // dialog flow
    bool offerToAddNewDatabaseElement(const DbSelectionComboBox *box);
    std::optional<int> addNewEntry(const DbSelectionComboBox *box);
    EntryEditDialog *getEntryEditDialog(const DbSelectionComboBox *box);
    bool setComboBoxValue(DbSelectionComboBox *box, int row_id);

    // UI Elements
    int m_eventId  = 0;
    int m_flightId = 0;
    const QString m_dateFormatString;
    const QString m_timeFormatString;

  private slots:
    void on_accepted();
    void on_unknown_value_entered(DbSelectionComboBox *box);
    void on_pilotFlyingCheckBoxStateChanged(Qt::CheckState state);
    void inline on_badInputReceived(QWidget *caller)
    {
        caller->setStyleSheet(OPL::CssStyles::RED_BORDER);
    }
    void inline on_GoodInputReceived(QWidget *caller) { caller->setStyleSheet(QString()); }
};

} // namespace OPL

#endif // FLIGHTLOGENTRYEDITDIALOG_H
