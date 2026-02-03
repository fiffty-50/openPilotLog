#include "tailselectionbox.h"

TailSelectionBox::TailSelectionBox(QWidget *parent)
    : DbSelectionComboBox(OPL::DbTable::v2AircraftTails, parent)
{
    refresh();
    connect(this->lineEdit(), &QLineEdit::textChanged, this,
            [this]() { this->lineEdit()->setText(this->lineEdit()->text().toUpper()); });

    this->setCurrentText({});
}
