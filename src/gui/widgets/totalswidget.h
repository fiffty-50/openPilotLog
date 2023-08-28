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
    const static int TOTALS_DATA_ROW_ID = OPL::STUB_ROW_ID;

    void fillTotals(const WidgetType widgetType);
    void fillStubData();
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
