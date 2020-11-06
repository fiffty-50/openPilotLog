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
#ifndef NEWPILOT_H
#define NEWPILOT_H

#include <QDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include "src/classes/pilot.h"

namespace Ui {
class NewPilot;
}

class NewPilot : public QDialog
{
    Q_OBJECT

public:
    explicit NewPilot(Db::editRole, QWidget *parent = nullptr);
    explicit NewPilot(Pilot, Db::editRole, QWidget *parent = nullptr);
    ~NewPilot();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::NewPilot *ui;

    Db::editRole role;

    Pilot oldEntry;

    void setupValidators();

    void formFiller();

    void submitForm();
};

#endif // NEWPILOT_H
