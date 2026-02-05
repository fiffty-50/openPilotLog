#ifndef AIRCRAFTTYPESELECTIONBOX_H
#define AIRCRAFTTYPESELECTIONBOX_H

#include "dbselectioncombobox.h"
#include "src/database/cache/aircrafttypesinfo.h"

class AircraftTypeSelectionBox : public DbSelectionComboBox {
    Q_OBJECT
  public:
    AircraftTypeSelectionBox(QWidget *parent = nullptr);

  protected:
    const QMap<QString, int> &getMap() const override { return aircraftTypesData->typeStringMap(); }
};

#endif // AIRCRAFTTYPESELECTIONBOX_H
