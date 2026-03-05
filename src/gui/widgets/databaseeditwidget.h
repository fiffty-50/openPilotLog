#ifndef DATABASEEDITWIDGET_H
#define DATABASEEDITWIDGET_H

#include "src/gui/widgets/tableeditwidget.h"
#include <QGridLayout>
#include <QTabWidget>
#include <QWidget>

class DatabaseEditWidget : public QWidget {
    Q_OBJECT
  public:
    explicit DatabaseEditWidget(QWidget *parent = nullptr);

    enum Table { Pilots, Tails, Airports, Aircraft, AirportCodes, ApproachTypes, Currencies };
    void addEntry(Table table);

  private:
    QGridLayout *gridLayout;
    QTabWidget *tabWidget;
    TableEditWidget *pilotTab;
    TableEditWidget *tailsTab;
    TableEditWidget *airportsTab;
    TableEditWidget *aircraftTab;
    TableEditWidget *approachTab;
    // TableEditWidget *airportCodesTab;
    // TableEditWidget *approachTypesTab;
    // TableEditWidget *currenciesTab;

    void setupUi();
    void retranslateUi();

  signals:
};

#endif // DATABASEEDITWIDGET_H
