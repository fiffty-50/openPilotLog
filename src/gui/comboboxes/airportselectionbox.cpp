#include "airportselectionbox.h"
#include "src/database/database.h"

AirportSelectionBox::AirportSelectionBox(QWidget *parent)
    : DbSelectionComboBox(OPL::DbTable::AirportCodes, parent)
{
    refresh();
    connect(DB, &OPL::Database::databaseUpdated, this, [this](OPL::DbTable table) {
        if (table == m_table) refresh();
    });
    this->setCurrentText({});
}
