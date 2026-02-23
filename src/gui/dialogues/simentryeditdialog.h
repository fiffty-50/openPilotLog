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
#ifndef SIMENTRYEDITDIALOG_H
#define SIMENTRYEDITDIALOG_H

#include "src/gui/dialogues/entryeditdialog.h"
#include "src/opl.h"
#include <QCalendarWidget>
#include <QComboBox>
#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>

/*!
 * \brief The SimEntryEditDialog class enables adding a new Simulator Session to the database or
 * editing an existing one.
 * \details The SimEntryEditDialog offers two constructors, one is used to create a new Simulator
 * Entry from scratch, while the other one is used to edit an existing entry. The existing entry is
 * identified by its ROW ID in the database and is then retreived, its data being used to pre-fill
 * the UI to enable editing the existing data.
 *
 * A QCompleter provides in-line completion for the aircraft type field.
 */
class SimEntryEditDialog : public EntryEditDialog {
    Q_OBJECT

  public:
    explicit SimEntryEditDialog(QWidget *parent = nullptr);
    explicit SimEntryEditDialog(int row_id, QWidget *parent = nullptr);
    ~SimEntryEditDialog() = default;

  private slots:
    void on_buttonBox_accepted();
    void on_dateLineEdit_editingFinished();
    void on_timeLineEdit_editingFinished();
    void on_helpPushButton_clicked();
    void on_registrationLineEdit_editingFinished();
    void on_datePushButton_clicked();
    void on_calendarDateSelected();

  private:
    // UI Elements
    QGridLayout *gridLayout;
    QLabel *simTypeLabel;
    QLabel *remarksLabel;
    QLabel *acftTypeLabel;
    QLabel *registrationLabel;
    QLabel *timeLabel;
    QLineEdit *registrationLineEdit;
    QLineEdit *timeLineEdit;
    QLineEdit *dateLineEdit;
    QLineEdit *acftTypeLineEdit;
    QLineEdit *remarksLineEdit;
    QComboBox *simTypeComboBox;
    QCalendarWidget *calendar;
    QPushButton *helpPushButton;
    QPushButton *dateButton;
    QDialogButtonBox *buttonBox;

    // Member Functions
    void init();
    void retranslateUi();
    void setupSlots();

    // TODO load from settings
    OPL::DateTimeFormat m_format =
        OPL::DateTimeFormat(OPL::DateTimeFormat::DateFormat::Default, QStringLiteral("yyyy-MM-dd"),
                            OPL::DateTimeFormat::TimeFormat::Default, QStringLiteral("hh:mm"));

    void fillEntryData();
    bool verifyInput(QString &error_msg);
    OPL::RowData_T collectInput();
    //OPL::SimulatorEntry entry;

    // EntryEditDialog interface
  public:
    void loadEntry(int rowID) override;
    bool deleteEntry(int rowID) override;
    void reset() override;
};

#endif // SIMENTRYEDITDIALOG_H
