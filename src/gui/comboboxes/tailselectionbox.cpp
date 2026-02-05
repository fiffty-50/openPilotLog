#include "tailselectionbox.h"
#include "src/database/database.h"

TailSelectionBox::TailSelectionBox(QWidget *parent)
    : DbSelectionComboBox(OPL::DbTable::v2AircraftTails, parent)
{
    refresh();
    connect(this->lineEdit(), &QLineEdit::textChanged, this,
            [this]() { this->lineEdit()->setText(this->lineEdit()->text().toUpper()); });
    // call refresh when the database table has been updated
    connect(DB, &OPL::Database::dataBaseUpdated, this, [this](OPL::DbTable table) {
        if (table == m_table) refresh();
    });

    this->setCurrentText({});
}
