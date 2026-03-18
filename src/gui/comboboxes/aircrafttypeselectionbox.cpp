#include "aircrafttypeselectionbox.h"
#include "src/database/database.h"

AircraftTypeSelectionBox::AircraftTypeSelectionBox(QWidget *parent)
    : DbSelectionComboBox(OPL::DbTable::AircraftTypes, parent)
{
    refresh();
    connect(DB, &OPL::Database::databaseUpdated, this, [this](OPL::DbTable table) {
        if (table == m_table) refresh();
    });
}
