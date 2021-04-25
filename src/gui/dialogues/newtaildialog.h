/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2021 Felix Turowsky
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
#include <QComboBox>

#include "src/classes/asettings.h"
#include "src/functions/acalc.h"
#include "src/database/adatabase.h"
#include "src/classes/atailentry.h"
#include "src/classes/aaircraftentry.h"

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
    //create new tail
    explicit NewTailDialog(QString new_registration, QWidget *parent = nullptr);
    //edit existing tail
    explicit NewTailDialog(int row_id, QWidget *parent = nullptr);

    ~NewTailDialog();
private:

    Ui::NewTail *ui;

    ATailEntry entry;

    QStringList aircraftList;

    QMap<QString, int> idMap;

    void setupCompleter();
    void setupValidators();
    void fillForm(AEntry entry, bool is_template);
    bool verify();
    void submitForm();

private slots:
    void on_operationComboBox_currentIndexChanged(int index);
    void on_ppTypeComboBox_currentIndexChanged(int index);
    void on_ppNumberComboBox_currentIndexChanged(int index);
    void on_weightComboBox_currentIndexChanged(int index);
    void on_registrationLineEdit_textChanged(const QString &arg1);
    void on_buttonBox_accepted();
    void onSearchCompleterActivated();

};

#endif // NEWTAIL_H
