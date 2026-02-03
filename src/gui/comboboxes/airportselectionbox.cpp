#include "airportselectionbox.h"

AirportSelectionBox::AirportSelectionBox(QWidget *parent)
    : DbSelectionComboBox(OPL::DbTable::v2AirportCodes, parent)
{
    refresh();
    this->setCurrentText({});
}
