#ifndef AIRCRAFTENTRYEDITDIALOG_H
#define AIRCRAFTENTRYEDITDIALOG_H

#include "entryeditdialog.h"

class AircraftEntryEditDialog : public EntryEditDialog
{
public:
    explicit AircraftEntryEditDialog(QWidget *parent = nullptr);
    explicit AircraftEntryEditDialog(int row_id, QWidget *parent = nullptr);

    // EntryEditDialog interface
    void loadEntry(int rowID) override;
    bool deleteEntry(int rowID) override;

private:
    void init();
    void retranslateUi();
    void loadAircraftData(int rowId);
};

#endif // AIRCRAFTENTRYEDITDIALOG_H
