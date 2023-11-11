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
#ifndef TOTALSWIDGET_H
#define TOTALSWIDGET_H

#include "QtWidgets/qlineedit.h"
#include "src/gui/verification/timeinput.h"
#include "src/opl.h"
#include <QWidget>
#include <QRegularExpressionValidator>

namespace Ui {
class TotalsWidget;
}

class TotalsWidget : public QWidget
{
    Q_OBJECT

public:
    enum WidgetType {TotalTimeWidget, PreviousExperienceWidget};
    explicit TotalsWidget(WidgetType widgetType, QWidget *parent = nullptr);
    ~TotalsWidget();



private:
    Ui::TotalsWidget *ui;
    /*!
     * \brief m_rowData holds the data displayed in the line edits
     */
    OPL::RowData_T m_rowData;
    /*!
     * \brief ROW_ID the row ID for previous experience entries (1)
     */

    OPL::DateTimeFormat m_format;
    const static int ROW_ID = 1;
    void fillTotals(const WidgetType widgetType);
    void setup(const WidgetType widgetType);
    void connectSignalsAndSlots();
    bool verifyUserTimeInput(QLineEdit *line_edit, const TimeInput &input);
    bool updateTimeEntry(const QLineEdit* line_edit);
    bool updateMovementEntry(const QLineEdit* line_edit);

private slots:
    void timeLineEditEditingFinished();
    void movementLineEditEditingFinished();
};

#endif // TOTALSWIDGET_H
