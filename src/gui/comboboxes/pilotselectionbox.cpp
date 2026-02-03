#include "pilotselectionbox.h"

PilotSelectionBox::PilotSelectionBox(QWidget *parent)
    : DbSelectionComboBox(OPL::DbTable::v2Pilots, parent)
{
    refresh();
    this->setCurrentText({});
}
