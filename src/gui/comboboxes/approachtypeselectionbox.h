#ifndef APPROACHTYPESELECTIONBOX_H
#define APPROACHTYPESELECTIONBOX_H

#include "dbselectioncombobox.h"
#include "src/database/cache/approachtypeinfo.h"

class ApproachTypeSelectionBox : public DbSelectionComboBox {
  public:
    ApproachTypeSelectionBox(QWidget *parent = nullptr);

  protected:
    const QMap<QString, int> &getMap() const override { return approachData->approachNamesMap(); }
};

#endif // APPROACHTYPESELECTIONBOX_H
