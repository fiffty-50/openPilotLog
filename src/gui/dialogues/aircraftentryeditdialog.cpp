#include "aircraftentryeditdialog.h"

AircraftEntryEditDialog::AircraftEntryEditDialog(QWidget *parent)
    : EntryEditDialog(0, parent)
{
    init();
}

AircraftEntryEditDialog::AircraftEntryEditDialog(int row_id, QWidget *parent)
    : EntryEditDialog(row_id, parent)
{
    init();
    loadAircraftData(row_id);
}

void AircraftEntryEditDialog::loadEntry(int rowID)
{

}

bool AircraftEntryEditDialog::deleteEntry(int rowID)
{
    return false;
}

void AircraftEntryEditDialog::init()
{

}

void AircraftEntryEditDialog::retranslateUi()
{

}

void AircraftEntryEditDialog::loadAircraftData(int rowId)
{

}
