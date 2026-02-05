#ifndef PILOTSELECTIONBOX_H
#define PILOTSELECTIONBOX_H

#include "dbselectioncombobox.h"
#include "src/database/pilotinfo.h"

class PilotSelectionBox : public DbSelectionComboBox {
  public:
    PilotSelectionBox(QWidget *parent = nullptr);

  protected:
    const QMap<QString, int> &getMap() const { return pilotsData->nameMap(); }
};

#endif // PILOTSELECTIONBOX_H
