#ifndef AIRPORTSELECTIONBOX_H
#define AIRPORTSELECTIONBOX_H

#include "dbselectioncombobox.h"
#include "src/database/airportinfo.h"

class AirportSelectionBox : public DbSelectionComboBox {
  public:
    AirportSelectionBox(QWidget *parent = nullptr);

  protected:
    const QMap<QString, int> &getMap() const { return airportData->allCodesMap(); }
};

#endif // AIRPORTSELECTIONBOX_H
