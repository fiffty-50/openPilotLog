/*
 *openPilot Log - A FOSS Pilot Logbook Application
 *Copyright (C) 2020  Felix Turowsky
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
#ifndef NEWTAIL_H
#define NEWTAIL_H

#include <QDialog>
#include <QCompleter>
#include <QMessageBox>
#include "src/classes/completionlist.h"
#include "src/classes/aircraft.h"

namespace Ui {
class NewTail;
}
/*!
 * \brief The NewTail class is a dialog for adding a new tail to
 * the database or editing an existing one.
 *
 * For a new tail, construct using QString reg.
 * For editing an existing tail, provide the aircraft object.
 */
class NewTail : public QDialog
{
    Q_OBJECT

public:
    explicit NewTail(aircraft acft, QWidget *parent = nullptr);

    explicit NewTail(QString reg, QWidget *parent = nullptr);

    explicit NewTail(db dbentry, sql::editRole edRole, QWidget *parent = nullptr);

    ~NewTail();


private slots:

    void on_searchLineEdit_textChanged(const QString &arg1);

    void on_buttonBox_accepted();

    void on_operationComboBox_currentIndexChanged(int index);

    void on_ppTypeComboBox_currentIndexChanged(int index);

    void on_ppNumberComboBox_currentIndexChanged(int index);

    void on_weightComboBox_currentIndexChanged(int index);

private:

    Ui::NewTail *ui;

    sql::editRole role;

    void formFiller(aircraft);

    void formFiller(db);

    bool verify();

    aircraft createAircraftFromSelection();

    void createEntry();

    QStringList aircraftlist;

    QMap<QString, int> idMap;

};

#endif // NEWTAIL_H
