#ifndef ENTRYEDITDIALOG_H
#define ENTRYEDITDIALOG_H

#include "src/database/row.h"
#include <QDialog>
#include <QObject>

/*!
 * \brief The EntryEditDialog class is a base class for Dialogs that enable editing of individual database entries
 */
class  EntryEditDialog : public QDialog
{
    Q_OBJECT
public:
    EntryEditDialog() = delete;
    EntryEditDialog(QWidget *parent = nullptr);
    EntryEditDialog(int rowID, QWidget *parent = nullptr);

    /*!
     * \brief load an entry from the database for editing
     * \param rowID - The row ID of the entry
     */
    virtual void loadEntry(int rowID) = 0;

    /*!
     * \brief load an entry for editing
     * \param entry - The (stub) entry.
     * \details This function can be used to edit an incomplete entry which may or may not exist in the
     * database yet.
     */
    virtual void loadEntry(OPL::Row entry) {
        TODO << "Implement in all subclasses";
        qApp->quit();
    }

    /*!
     * \brief delete an entry from the database
     * \param rowID - the row ID to be deleted
     * \return true on success
     */
    virtual bool deleteEntry(int rowID) = 0;

protected:
    int rowID;
};

#endif // ENTRYEDITDIALOG_H
