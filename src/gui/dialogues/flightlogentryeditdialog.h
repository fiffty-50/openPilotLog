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
#include "src/database/databasecache.h"
#include "src/gui/comboboxes/dbselectioncombobox.h"
#include "src/gui/verification/flightdatabuilder.h"
#include "src/opl.h"
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
#include <qtmetamacros.h>

namespace OPL {

using MapType = DatabaseCache::MapType;

class FlightLogEntryEditDialog : public EntryEditDialog {
  public:
    explicit FlightLogEntryEditDialog(QWidget *parent = nullptr);
    void loadEntry(int rowID);
    bool deleteEntry(int rowID);

  private:
    // dialog setup
    void init();
    void retranslateUi();
    void setupValidationAndCompletion();
    void setupSlots();
    void readSettings();

    // dialog flow
    bool addNewEntry(DbSelectionComboBox *box);
    bool addNewDatabaseElement(DbSelectionComboBox *box);
    bool m_addNewOffered = false; // de-bounce repeated triggering of editing finished by QCompleter
    bool m_addNewDialogExecuted = false;

    // data collection and verification
    bool runSanityChecks();
    FlightDataBuilder collectFlightDataFromUi();

    // UI Elements
    QGridLayout *gridLayout;
    QLabel *dateDisplayLabel;
    QLabel *deptLabel;
    QLabel *deptDisplayLabel;
    QLabel *destLabel;
    QLabel *destDisplayLabel;
    QLabel *timeOffLabel;
    QLabel *timeOnLabel;
    QLabel *totalTimeLabel;
    QLabel *totalTimeDisplayLabel;
    QLabel *pilotFunctionLabel;
    QLabel *flightRulesLabel;
    QLabel *registrationLabel;
    QLabel *picLabel;
    QLabel *sicLabel;
    QLabel *flightNumberLabel;
    QLabel *takeOffCountLabel;
    QLabel *landingCountLabel;
    QLabel *remarksLabel;
    QTimeEdit *timeOffEdit;
    QTimeEdit *timeOnEdit;
    DbSelectionComboBox *deptComboBox;
    DbSelectionComboBox *destComboBox;
    DbSelectionComboBox *registrationComboBox;
    DbSelectionComboBox *picComboBox;
    DbSelectionComboBox *sicComboBox;
    QLineEdit *flightNumberLineEdit;
    QPushButton *datePushButton;
    QCheckBox *pilotFlyingCheckBox;
    QDateEdit *dateEdit;
    QComboBox *pilotFunctionComboBox;
    QComboBox *flightRulesComboBox;
    QSpinBox *takeOffCountSpinBox;
    QSpinBox *landingCountSpinBox;
    QPlainTextEdit *remarksTextEdit;
    QDialogButtonBox *buttonBox;

    QList<QLineEdit *> m_locationLineEdits;
    int m_eventId = 0;
    double m_night_angle;

    const QString m_dateFormatString;
    const QString m_timeFormatString;

  private slots:
    void on_accepted();
    void on_selectionComboBox_unkownValueEntered(DbSelectionComboBox *caller);
    void on_pilotFlyingCheckBoxStateChanged(Qt::CheckState state);
    void inline on_badInputReceived(QWidget *caller)
    {
        caller->setStyleSheet(OPL::CssStyles::RED_BORDER);
    }
    void inline on_GoodInputReceived(QWidget *caller) { caller->setStyleSheet(QString()); }
};

} // namespace OPL

#endif // FLIGHTLOGENTRYEDITDIALOG_H
