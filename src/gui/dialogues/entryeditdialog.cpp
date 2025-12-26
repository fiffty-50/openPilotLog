#include "entryeditdialog.h"

EntryEditDialog::EntryEditDialog(QWidget *parent)
    : QDialog{parent}
{
    m_rowId = 0;
}

EntryEditDialog::EntryEditDialog(int rowID, QWidget *parent)
    : QDialog{parent}, m_rowId(rowID)
{

}
