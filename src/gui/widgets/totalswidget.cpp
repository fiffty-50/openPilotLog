/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2023 Felix Turowsky
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
#include "totalswidget.h"
#include "QtWidgets/qlineedit.h"
#include "src/classes/settings.h"
#include "src/database/database.h"
#include "src/opl.h"
#include "ui_totalswidget.h"

TotalsWidget::TotalsWidget(WidgetType widgetType, QWidget *parent)
    : QWidget(parent), ui(new Ui::TotalsWidget)
{
    ui->setupUi(this);
    setup(widgetType);
}

TotalsWidget::~TotalsWidget() { delete ui; }

/*!
 * \brief TotalsWidget::setup Sets the line edits as editable or read-only and connects signals if
 * required
 * \details This widget can be used to either display the totals (in the home widget) or
 * to edit the total previous experience, from previous logbooks (in the settings widget).
 */
void TotalsWidget::setup(const WidgetType widgetType)
{
    m_format                           = OPL::DateTimeFormat();
    const QList<QLineEdit *> lineEdits = this->findChildren<QLineEdit *>();

    switch (widgetType) {
    case TotalTimeWidget:
        LOG << "Setting up totals widget";
        // disable editing
        for (const auto &lineEdit : lineEdits) {
            lineEdit->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        }
        // populate the UI
        fillTotals(widgetType);
        break;
    case PreviousExperienceWidget:
        LOG << "Setting up previous XP widget";
        for (const auto &lineEdit : lineEdits) {
            lineEdit->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
            // set a validator for the TO/LDG line edits, the other ones get validated seperately
            if (lineEdit->objectName().contains(QLatin1String("to")) ||
                lineEdit->objectName().contains(QLatin1String("ldg"))) {
                lineEdit->setValidator(new QIntValidator(0, std::numeric_limits<int>::max(), this));
            }
        }
        // initialise m_rowData
        //m_rowData = DB->getRowData(OPL::DbTable::PreviousExperience, ROW_ID);

        // populate the UI
        fillTotals(widgetType);
        connectSignalsAndSlots();
        break;
    default:
        break;
    }
}

/*!
 * \brief HomeWidget::fillTotals Retreives a Database Summary of Total Flight Time and fills the UI.
 */
void TotalsWidget::fillTotals(const WidgetType widgetType)
{
}

/*!
 * \brief TotalsWidget::connectSignalsAndSlots If the widget is editable, connects the signals and
 * slots
 */
void TotalsWidget::connectSignalsAndSlots()
{

}

void TotalsWidget::timeLineEditEditingFinished()
{

}

void TotalsWidget::movementLineEditEditingFinished()
{

}
