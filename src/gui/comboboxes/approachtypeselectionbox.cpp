#include "approachtypeselectionbox.h"
#include "src/database/database.h"

ApproachTypeSelectionBox::ApproachTypeSelectionBox(QWidget *parent)
    : DbSelectionComboBox(OPL::DbTable::ApproachTypes, parent)
{
    refresh();
    connect(DB, &OPL::Database::dataBaseUpdated, this, [this](OPL::DbTable table) {
        if (table == m_table) refresh();
    });
    this->setCurrentText({});
}
