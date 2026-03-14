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
#ifndef FLIGHTENTRYEDITUI_H
#define FLIGHTENTRYEDITUI_H

#include "src/gui/comboboxes/dbselectioncombobox.h"
#include <QCheckBox>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>

namespace Ui {

class FlightEntryEditUi {
  public:
    void setupUi(QWidget *parent = nullptr);
    void retranslateUi();

    QGridLayout *gridLayout;
    QLabel *dateLabel;
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
    QCheckBox *pilotFlyingCheckBox;
    QDateEdit *dateEdit;
    QComboBox *pilotFunctionComboBox;
    QComboBox *flightRulesComboBox;
    QSpinBox *takeOffCountSpinBox;
    QSpinBox *landingCountSpinBox;
    QPlainTextEdit *remarksTextEdit;
    QDialogButtonBox *buttonBox;

  private:
    void createLayout(QWidget *parent = nullptr);
};

} // namespace Ui

#endif // FLIGHTENTRYEDITUI_H
