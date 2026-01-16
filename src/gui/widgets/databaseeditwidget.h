#ifndef DATABASEEDITWIDGET_H
#define DATABASEEDITWIDGET_H

#include "src/gui/widgets/tableeditwidget.h"
#include <QWidget>
#include <QTabWidget>
#include <QGridLayout>

class DatabaseEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DatabaseEditWidget(QWidget *parent = nullptr);

private:
    QGridLayout *gridLayout;
    QTabWidget *tabWidget;
    TableEditWidget *pilotTab;
    TableEditWidget *tailsTab;
    TableEditWidget *airportsTab;
    TableEditWidget *aircraftTab;
    // TableEditWidget *airportCodesTab;
    // TableEditWidget *approachTypesTab;
    // TableEditWidget *currenciesTab;

    void setupUi();
    void retranslateUi();

signals:
};

#endif // DATABASEEDITWIDGET_H
