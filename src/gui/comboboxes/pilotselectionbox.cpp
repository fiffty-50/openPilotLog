#include "pilotselectionbox.h"
#include "src/database/database.h"

PilotSelectionBox::PilotSelectionBox(QWidget *parent)
    : DbSelectionComboBox(OPL::DbTable::Pilots, parent)
{
    refresh();
    connect(DB, &OPL::Database::dataBaseUpdated, this, [this](OPL::DbTable table) {
        if (table == m_table) refresh();
    });
    this->setCurrentText({});
}
