#ifndef TOTALSWIDGET_H
#define TOTALSWIDGET_H

#include <QWidget>

namespace Ui {
class TotalsWidget;
}

class TotalsWidget : public QWidget
{
    Q_OBJECT
    enum WidgetType {TotalTimeWidget, PreviousExperienceWidget};

public:
    explicit TotalsWidget(WidgetType widgetType, QWidget *parent = nullptr);
    ~TotalsWidget();



private:
    Ui::TotalsWidget *ui;
    void fillTotals(WidgetType widgetType);
    void setup(WidgetType widgetType);
    void connectSignalsAndSlots();
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
