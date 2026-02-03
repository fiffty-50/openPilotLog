#ifndef AIRPORTSELECTIONBOX_H
#define AIRPORTSELECTIONBOX_H

#include "dbselectioncombobox.h"

class AirportSelectionBox : public DbSelectionComboBox {
  public:
    AirportSelectionBox(QWidget *parent = nullptr);

  protected:
    QString getQuery() const
    {
        return QStringLiteral("WITH CurrentCode AS ( "
                              "SELECT "
                              "airport_id, "
                              "airport_code, "
                              "valid_from_jd, "
                              "valid_to_jd "
                              "FROM airport_codes "
                              "WHERE (valid_to_jd IS NULL OR valid_to_jd >= julianday('now')) "
                              "AND valid_from_jd <= julianday('now')) "
                              "SELECT "
                              "airport_id, "
                              "airport_code "
                              "FROM CurrentCode ");
    }
};

#endif // AIRPORTSELECTIONBOX_H
