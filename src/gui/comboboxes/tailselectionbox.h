#ifndef TAILSELECTIONBOX_H
#define TAILSELECTIONBOX_H

#include "dbselectioncombobox.h"
#include "src/database/cache/tailregistrationsinfo.h"

class TailSelectionBox : public DbSelectionComboBox {
  public:
    TailSelectionBox(QWidget *parent = nullptr);

  protected:
    const QMap<QString, int> &getMap() const { return tailsData->registrationsMap(); }
};

#endif // TAILSELECTIONBOX_H
