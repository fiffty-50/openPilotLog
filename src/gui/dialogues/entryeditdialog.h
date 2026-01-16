/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2026 Felix Turowsky
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef ENTRYEDITDIALOG_H
#define ENTRYEDITDIALOG_H

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
     * \brief delete an entry from the database
     * \param rowID - the row ID to be deleted
     * \return true on success
     */
    virtual bool deleteEntry(int rowID) = 0;

protected:
    int m_rowId;
};

#endif // ENTRYEDITDIALOG_H
