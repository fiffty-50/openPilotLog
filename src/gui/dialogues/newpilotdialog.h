/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2023 Felix Turowsky
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
#ifndef NEWPILOT_H
#define NEWPILOT_H

#include <QDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QCompleter>
#include "src/database/pilotentry.h"
#include "src/gui/dialogues/entryeditdialog.h"

namespace Ui {
class NewPilot;
}
/*!
 * \brief The NewPilotDialog enables adding new entries to the pilot table in the database
 * \details The NewPilotDialog offers two constructors, one is used to create a new Pilot Entry
 * from scratch, while the other one is used to edit an existing entry. The existing entry
 * is identified by its ROW ID in the database and is then retreived, its data being used
 * to pre-fill the UI to enable editing the existing data.
 *
 * A QCompleter provides in-line completion for the company field.
 *
 * No verification is done on the user input. Sqlite has full unicode support and names
 * come in all different forms and shapes around the world. In order to maintain a maximum
 * amount of flexibility, any unicode input is allowed.
 */
class NewPilotDialog : public EntryEditDialog
{
    Q_OBJECT
public:
    explicit NewPilotDialog(QString userInput = QString(), QWidget *parent = nullptr);
    explicit NewPilotDialog(int rowId, QWidget *parent = nullptr);
    ~NewPilotDialog();




private slots:
    void on_buttonBox_accepted();
private:
    Ui::NewPilot *ui;

    OPL::PilotEntry pilotEntry;

    inline void setup();

    /*!
     * \brief formFiller - fills the line edits with the data retreived from the database.
     */
    void formFiller();

    /*!
     * \brief submitForm - retreives the input from the line edits and commits to the database.
     */
    void submitForm();

    // EntryEditDialog interface
public:
    virtual bool deleteEntry(int rowId) override;
    virtual void loadEntry(int rowId) override;

};


#endif // NEWPILOT_H
