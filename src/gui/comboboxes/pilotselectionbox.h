#ifndef PILOTSELECTIONBOX_H
#define PILOTSELECTIONBOX_H

#include "dbselectioncombobox.h"

class PilotSelectionBox : public DbSelectionComboBox {
  public:
    PilotSelectionBox(QWidget *parent = nullptr);

  protected:
    QString getQuery() const override
    {
        return QStringLiteral("SELECT pilot_id, pilot_name FROM pilots");
    }
};

#endif // PILOTSELECTIONBOX_H
