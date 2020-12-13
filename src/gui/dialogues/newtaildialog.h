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
#include <QRegularExpression>

#include "src/classes/settings.h"
#include "src/classes/aircraft.h"
#include "src/classes/strictrxvalidator.h"
#include "src/classes/acalc.h"
#include "src/database/entry.h"
#include "src/experimental/DataBase.h"

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
class NewTailDialog : public QDialog
{
    Q_OBJECT

public:
    //to create new tail from scratch
    explicit NewTailDialog(QString reg, Db::editRole edRole, QWidget *parent = nullptr);
    //to edit existing tail
    explicit NewTailDialog(Aircraft dbentry, Db::editRole edRole, QWidget *parent = nullptr);

    ~NewTailDialog();
private slots:

    void on_searchLineEdit_textChanged(const QString &arg1);

    void on_buttonBox_accepted();

    void on_operationComboBox_currentIndexChanged(int index);

    void on_ppTypeComboBox_currentIndexChanged(int index);

    void on_ppNumberComboBox_currentIndexChanged(int index);

    void on_weightComboBox_currentIndexChanged(int index);

    void on_registrationLineEdit_textChanged(const QString &arg1);

private:

    Ui::NewTail *ui;

    Db::editRole role;

    Aircraft oldEntry;

    QStringList aircraftlist;

    QMap<QString, int> idMap;

    void submitForm(Db::editRole edRole);

    void setupCompleter();

    void setupValidators();

    void formFiller(Entry);

    bool verify();
};

#endif // NEWTAIL_H
