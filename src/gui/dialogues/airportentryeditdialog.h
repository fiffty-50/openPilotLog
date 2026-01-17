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
#ifndef AIRPORTENTRYEDITDIALOG_H
#define AIRPORTENTRYEDITDIALOG_H

#include "src/gui/dialogues/entryeditdialog.h"
#include <QGridLayout>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QComboBox>

class AirportEntryEditDialog : public EntryEditDialog
{
    Q_OBJECT

public:
    explicit AirportEntryEditDialog(QWidget *parent = nullptr);
    explicit AirportEntryEditDialog(int row_id, QWidget* parent = nullptr);

private slots:
    void on_editIcaoCodePushButton_clicked();
    void on_editIataCodePushButton_clicked();
    void on_editOtherCodePushButton_clicked();
    void on_buttonBox_accepted();

private:
    // UI Elements
    QGridLayout *gridLayout;
    QDialogButtonBox *buttonBox;
    QLineEdit *nameLineEdit;
    QLabel *nameLabel;
    QLabel *longitudeLabel;
    QLabel *latitudeLabel;
    QLabel *timezoneLabel;
    QLabel *icaoLabel;
    QLabel *icaoDisplayLabel;
    QLabel *iataLabel;
    QLabel *iataDisplayLabel;
    QLabel *otherCodeLabel;
    QLabel *otherCodeDisplayLabel;
    QPushButton *editIcaoCodePushButton;
    QPushButton *editIataCodePushButton;
    QPushButton *editOtherCodePushButton;
    QDoubleSpinBox *lonDoubleSpinBox;
    QDoubleSpinBox *latDoubleSpinBox;
    QComboBox *timeZoneComboBox;

    // Member Variables
    int m_rowId;

    // Member Functions
    void init();
    void retranslateUi();
    void setupSlots();
    void loadTimeZones();
    bool confirmTimezone();
    void loadAirportData(int row_id);
    bool userWantsToEditCode();


    // EntryEditDialog interface
public:
    void loadEntry(int rowId) override;
    bool deleteEntry(int rowId) override;
};

#endif // AIRPORTENTRYEDITDIALOG_H
