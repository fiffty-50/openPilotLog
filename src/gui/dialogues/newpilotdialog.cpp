/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2022 Felix Turowsky
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
#include "newpilotdialog.h"
#include "ui_newpilot.h"
#include "src/opl.h"

#include "src/database/database.h"
#include "src/database/dbcompletiondata.h"
#include "src/database/row.h"
#include "src/functions/alog.h"

/*!
 * \brief NewPilotDialog::NewPilotDialog - creates a new pilot dialog which can be used to add a new entry to the database
 */
NewPilotDialog::NewPilotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewPilot)
{
    setup();

    //pilotEntry = APilotEntry();
    ui->lastnameLineEdit->setFocus();
}

/*!
 * \brief NewPilotDialog::NewPilotDialog - creates a new pilot dialog which can be used to edit an existing entry in the database
 * \param rowId - the rowid of the entry to be edited in the database
 */
NewPilotDialog::NewPilotDialog(int rowId, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewPilot)
{
    setup();

    pilotEntry = DB->getPilotEntry(rowId);
    DEB << "Editing Pilot: " << pilotEntry;
    formFiller();
    ui->lastnameLineEdit->setFocus();
}

NewPilotDialog::~NewPilotDialog()
{
    delete ui;
}

void NewPilotDialog::setup()
{
    ui->setupUi(this);

    auto completer = new QCompleter(OPL::DbCompletionData::getCompletionList(OPL::CompleterTarget::Companies), ui->companyLineEdit);
    completer->setCompletionMode(QCompleter::InlineCompletion);
    completer->setCaseSensitivity(Qt::CaseSensitive);
    ui->companyLineEdit->setCompleter(completer);
}

void NewPilotDialog::on_buttonBox_accepted()
{
    if (ui->lastnameLineEdit->text().isEmpty() || ui->firstnameLineEdit->text().isEmpty()) {
        QMessageBox message_box(this);
        message_box.setText(tr("Last Name and First Name are required."));
        message_box.exec();
    } else {
        submitForm();
    }
}

void NewPilotDialog::formFiller()
{
    auto line_edits = this->findChildren<QLineEdit *>();

    for (const auto &le : line_edits) {
        auto key = le->objectName().remove(QStringLiteral("LineEdit"));
        le->setText(pilotEntry.getData().value(key).toString());
    }
}

void NewPilotDialog::submitForm()
{
    RowData_T new_data;
    const auto line_edits = this->findChildren<QLineEdit *>();
    for(auto& le : line_edits) {
        auto key = le->objectName().remove(QStringLiteral("LineEdit"));
        auto value = le->text();
        new_data.insert(key, value);
    }

    pilotEntry.setData(new_data);
    DEB << "Submitting Pilot:";
    DEB << pilotEntry;
    if (!DB->commit(pilotEntry)) {
        QMessageBox message_box(this);
        message_box.setText(tr("The following error has ocurred:"
                               "<br><br>%1<br><br>"
                               "The entry has not been saved."
                               ).arg(DB->lastError.text()));
        message_box.exec();
        return;
    } else {
        QDialog::accept();
    }
}
