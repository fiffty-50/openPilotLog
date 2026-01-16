#include "databaseeditwidget.h"
#include "src/gui/widgets/aircrafttableeditwidget.h"
#include "src/gui/widgets/pilottableeditwidget.h"
#include "src/gui/widgets/tailtableeditwidget.h"
#include "src/gui/widgets/airporttableeditwidget.h"
#include "src/gui/widgets/tailtableeditwidget.h"


DatabaseEditWidget::DatabaseEditWidget(QWidget *parent)
    : QWidget{parent}
{
    setupUi();
    retranslateUi();
    tabWidget->setCurrentIndex(0);
}

void DatabaseEditWidget::setupUi()
{
    gridLayout = new QGridLayout(this);
    tabWidget = new QTabWidget(this);

    pilotTab = new PilotTableEditWidget(this);
    pilotTab->init();
    tabWidget->addTab(pilotTab, {});

    tailsTab = new TailTableEditWidget(this);
    tailsTab->init();
    tabWidget->addTab(tailsTab, {});

    aircraftTab = new AircraftTableEditWidget(this);
    aircraftTab->init();
    tabWidget->addTab(aircraftTab, {});

    airportsTab = new AirportTableEditWidget(this);
    airportsTab->init();
    tabWidget->addTab(airportsTab, {});



    gridLayout->addWidget(tabWidget, 0, 0, 1, 1);
}

void DatabaseEditWidget::retranslateUi()
{
    tabWidget->setTabText(tabWidget->indexOf(pilotTab), tr("Pilots"));
    tabWidget->setTabText(tabWidget->indexOf(tailsTab), tr("Tails"));
    tabWidget->setTabText(tabWidget->indexOf(aircraftTab), tr("Aircraft"));
    tabWidget->setTabText(tabWidget->indexOf(airportsTab), tr("Airports"));
}
