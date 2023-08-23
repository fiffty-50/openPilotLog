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
    OPL::RowData_T m_rowData;
    const QRegularExpressionValidator *m_timeValidator = new QRegularExpressionValidator(QRegularExpression("([01]?[0-9]|2[0-3]):?[0-5][0-9]?"), this);
    const static int TOTALS_DATA_ROW_ID = 1;

    void fillTotals(WidgetType widgetType);
    void setup(WidgetType widgetType);
    void connectSignalsAndSlots();
    bool verifyUserTimeInput(QLineEdit *line_edit, const TimeInput &input);
    bool updateDatabase(const QString &db_field, const QString &value);

private slots:
    void totalTimeEdited();
    void spseTimeEdited();
    void spmeTimeEdited();
    void multipilotTimeEdited();
    void picTimeEdited();
    void sicTimeEdited();
    void dualTimeEdited();
    void fiTimeEdited();
    void picusTimeEdited();
    void ifrTimeEdited();
    void nightTimeEdited();
    void simulatorTimeEdited();
    void toDayEdited();
    void toNightedited();
    void ldgDayEdited();
    void ldgNightEdited();
};

#endif // TOTALSWIDGET_H
