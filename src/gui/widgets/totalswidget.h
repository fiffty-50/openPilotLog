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
     * \brief ROW_ID the row ID for previous experience entries (0)
     */
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
