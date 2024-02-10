#include "entryeditdialog.h"

EntryEditDialog::EntryEditDialog(QWidget *parent)
    : QDialog{parent}
{
    rowID = 0;
}

EntryEditDialog::EntryEditDialog(int rowID, QWidget *parent)
    : QDialog{parent}, rowID(rowID)
{

}
