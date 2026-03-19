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
#include "flightentryeditui.h"
#include "src/gui/comboboxes/airportselectionbox.h"
#include "src/gui/comboboxes/approachtypeselectionbox.h"
#include "src/gui/comboboxes/pilotselectionbox.h"
#include "src/gui/comboboxes/tailselectionbox.h"
#include <QObject>

namespace Ui {

void FlightEntryEditUi::setupUi(QWidget *parent)
{
    createLayout(parent);
    retranslateUi();
}

void FlightEntryEditUi::retranslateUi()
{
    dateLabel->setText(QObject::tr("Date"));
    dateDisplayLabel->setText(dateEdit->date().toString(QStringLiteral("ddd d MMM, yyyy")));
    deptLabel->setText(QObject::tr("Departure"));
    destLabel->setText(QObject::tr("Destination"));
    timeOffLabel->setText(QObject::tr("Off Blocks"));
    timeOnLabel->setText(QObject::tr("On Blocks"));
    totalTimeLabel->setText(QObject::tr("Total"));
    pilotFunctionLabel->setText(QObject::tr("Function"));
    flightRulesLabel->setText(QObject::tr("Flight Rules"));
    registrationLabel->setText(QObject::tr("Registration"));
    picLabel->setText(QObject::tr("Pilot in Command"));
    sicLabel->setText(QObject::tr("Second Pilot"));
    flightNumberLabel->setText(QObject::tr("Flight Number"));
    pilotFlyingLabel->setText(QObject::tr("Pilot Flying"));
    takeOffCountLabel->setText(QObject::tr("Take Off"));
    landingCountLabel->setText(QObject::tr("Landing"));
    approachLabel->setText(QObject::tr("Approach Type"));
    remarksLabel->setText(QObject::tr("Remarks"));
    totalTimeDisplayLabel->setText(QStringLiteral("00:00"));
}

void FlightEntryEditUi::createLayout(QWidget *parent)
{
    // Main Layout
    // 5 columns with the middle column used as a spacer or in some cases for display Labels
    gridLayout         = new QGridLayout(parent);
    int row            = 0;
    constexpr int col0 = 0;
    constexpr int col1 = 1;
    constexpr int col2 = 2; // middle separator
    constexpr int col3 = 3;
    constexpr int col4 = 4;

    constexpr int singleSpan    = 1;
    constexpr int spanRemaining = -1;

    // Left side (cols 0–1), optionally fills col 2
    auto addLeft = [&](QWidget *left, QWidget *right, QWidget *middle = nullptr) {
        gridLayout->addWidget(left, row, col0, singleSpan, singleSpan);
        gridLayout->addWidget(right, row, col1, singleSpan, singleSpan);

        if (middle)
            gridLayout->addWidget(middle, row, col2, singleSpan, singleSpan);
        else
            gridLayout->addWidget(new QLabel(parent), row, col2, singleSpan, singleSpan);
    };

    // Right side (cols 3–4), advances row
    auto addRight = [&](QWidget *left, QWidget *right) {
        gridLayout->addWidget(left, row, col3, singleSpan, singleSpan);
        gridLayout->addWidget(right, row, col4, singleSpan, singleSpan);
        row++;
    };

    // Row
    // Left
    dateLabel        = new QLabel(parent);
    dateEdit         = new QDateEdit(parent);
    dateDisplayLabel = new QLabel(parent);
    addLeft(dateLabel, dateEdit, dateDisplayLabel);

    // Right
    registrationLabel = new QLabel(parent);
    // registrationLineEdit = new QLineEdit(parent);
    registrationComboBox = new TailSelectionBox(parent);
    addRight(registrationLabel, registrationComboBox);

    // Row
    // Left
    deptLabel        = new QLabel(parent);
    deptComboBox     = new AirportSelectionBox(parent);
    deptDisplayLabel = new QLabel(parent);
    addLeft(deptLabel, deptComboBox, deptDisplayLabel);

    // Right
    picLabel    = new QLabel(parent);
    picComboBox = new PilotSelectionBox(parent);
    addRight(picLabel, picComboBox);

    // Row
    // Left
    destLabel        = new QLabel(parent);
    destComboBox     = new AirportSelectionBox(parent);
    destDisplayLabel = new QLabel(parent);
    addLeft(destLabel, destComboBox, destDisplayLabel);

    // Right
    sicLabel    = new QLabel(parent);
    sicComboBox = new PilotSelectionBox(parent);
    addRight(sicLabel, sicComboBox);

    // Row
    // Left
    timeOffLabel = new QLabel(parent);
    timeOffEdit  = new QTimeEdit(parent);
    addLeft(timeOffLabel, timeOffEdit);

    // Right
    pilotFlyingLabel = new QLabel(parent);
    pilotFlyingCheckBox = new QCheckBox(parent);
    addRight(pilotFlyingLabel, pilotFlyingCheckBox);
    //gridLayout->addWidget(pilotFlyingCheckBox, row, col3, singleSpan, spanRemaining);
    row++;

    // Row
    // Left
    timeOnLabel = new QLabel(parent);
    timeOnEdit  = new QTimeEdit(parent);
    addLeft(timeOnLabel, timeOnEdit);

    // Right
    approachLabel = new QLabel(parent);
    approachBox   = new ApproachTypeSelectionBox(parent);
    addRight(approachLabel, approachBox);

    // Row
    // Left
    pilotFunctionLabel    = new QLabel(parent);
    pilotFunctionComboBox = new QComboBox(parent);
    addLeft(pilotFunctionLabel, pilotFunctionComboBox);

    // Right
    takeOffCountLabel   = new QLabel(parent);
    takeOffCountSpinBox = new QSpinBox(parent);
    addRight(takeOffCountLabel, takeOffCountSpinBox);

    // Row
    // Left
    flightRulesLabel    = new QLabel(parent);
    flightRulesComboBox = new QComboBox(parent);
    addLeft(flightRulesLabel, flightRulesComboBox);

    // Right
    landingCountLabel   = new QLabel(parent);
    landingCountSpinBox = new QSpinBox(parent);
    addRight(landingCountLabel, landingCountSpinBox);

    // Row
    // Left
    flightNumberLabel    = new QLabel(parent);
    flightNumberLineEdit = new QLineEdit(parent);
    addLeft(flightNumberLabel, flightNumberLineEdit);

    // Right
    remarksLabel    = new QLabel(parent);
    remarksTextEdit = new QPlainTextEdit(parent);
    remarksTextEdit->setMaximumHeight(flightNumberLineEdit->sizeHint().height() * 2);
    addRight(remarksLabel, remarksTextEdit);

    // Row
    // Right
    totalTimeLabel        = new QLabel(parent);
    totalTimeDisplayLabel = new QLabel(parent);
    addRight(totalTimeLabel, totalTimeDisplayLabel);

    // Row
    buttonBox = new QDialogButtonBox(parent);
    // buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
    gridLayout->addWidget(buttonBox, row, col4, singleSpan, singleSpan);
}

} // namespace Ui
