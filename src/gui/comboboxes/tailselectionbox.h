#ifndef TAILSELECTIONBOX_H
#define TAILSELECTIONBOX_H

#include "dbselectioncombobox.h"

class TailSelectionBox : public DbSelectionComboBox {
  public:
    TailSelectionBox(QWidget *parent = nullptr);

  protected:
    QString getQuery() const override
    {
        return QStringLiteral("SELECT tail_id, registration FROM aircraft_tails");
    }
};

#endif // TAILSELECTIONBOX_H
