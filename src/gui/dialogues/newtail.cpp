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
#include "newtail.h"
#include "ui_newtail.h"

// Debug Makro
#define DEB(expr) \
    qDebug() << __PRETTY_FUNCTION__<< "\t" << expr


//Dialog to be used to create a new tail
NewTail::NewTail(QString newreg, db::editRole edRole, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewTail)
{
    ui->setupUi(this);
    role = edRole;
    setupCompleter();

    ui->editLabel->hide();
    ui->registrationLineEdit->setText(newreg);
    ui->searchLineEdit->setStyleSheet("border: 1px solid blue");
    ui->searchLineEdit->setFocus();
}

//Dialog to be used to edit an existing tail
NewTail::NewTail(aircraft dbentry, db::editRole edRole, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewTail)
{
    oldEntry = dbentry;
    ui->setupUi(this);
    role = edRole;
    ui->searchLabel->hide();
    ui->searchLineEdit->hide();
    ui->line->hide();
    formFiller(dbentry);
}

NewTail::~NewTail()
{
    delete ui;
}

/*!
 * \brief NewTail::formFiller populates the Dialog with the
 * information contained in an aircraft object.
 * \param db - entry retreived from database
 */
void NewTail::formFiller(aircraft entry)
{
    DEB("Filling Form for a/c" << entry);
    //fill Line Edits
    auto line_edits = parent()->findChildren<QLineEdit *>();
    for (const auto &le : line_edits) {
        QString name = le->objectName();
        name.chop(8);//remove "LineEdit"
        QString value = entry.data.value(name);
        if (!value.isEmpty()) {
            le->setText(value);
        };
    }
    //select comboboxes
    QVector<QString> operation = {entry.data.value("singleengine"), entry.data.value("multiengine")};
    QVector<QString> ppNumber =  {entry.data.value("singlepilot"), entry.data.value("multipilot")};
    QVector<QString> ppType =    {entry.data.value("unpowered"), entry.data.value("piston"),
                                  entry.data.value("turboprop"), entry.data.value("jet")
                                 };
    QVector<QString> weight =    {entry.data.value("light"), entry.data.value("medium"),
                                  entry.data.value("heavy"), entry.data.value("super")
                                 };


    ui->operationComboBox->setCurrentIndex(operation.indexOf("1") + 1);
    ui->ppNumberComboBox->setCurrentIndex(ppNumber.indexOf("1") + 1);
    ui->ppTypeComboBox->setCurrentIndex(ppType.indexOf("1") + 1);
    ui->weightComboBox->setCurrentIndex(weight.indexOf("1") + 1);
}

/// Functions

/*!
 * \brief NewTail::setupCompleter creates a QMap<aircaft_id,searchstring> for auto completion,
 * obtains a QStringList for QCompleter and sets up search line edit accordingly
 */
void NewTail::setupCompleter()
{
    auto query = QLatin1String("SELECT make||' '||model||'-'||variant, aircraft_id FROM aircraft");
    auto vector = db::customQuery(query, 2);
    QMap<QString, int> map;
    for (int i = 0; i < vector.length() - 2 ; i += 2) {
        if (vector[i] != QLatin1String("")) {
            map.insert(vector[i], vector[i + 1].toInt());
        }
    }
    //creating QStringlist for QCompleter
    auto cl = new completionList(completerTarget::aircraft);

    aircraftlist = cl->list;
    idMap = map;

    QCompleter *completer = new QCompleter(aircraftlist, ui->searchLineEdit);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setFilterMode(Qt::MatchContains);
    ui->searchLineEdit->setCompleter(completer);
}
/*!
 * \brief NewTail::verify A simple check for empty recommended fields in the form
 * \return true if all reconmmended fields are populated
 */
bool NewTail::verify()
{
    auto recommended_line_edits = parent()->findChildren<QLineEdit *>("registrationLineEdit");
    recommended_line_edits.append(parent()->findChild<QLineEdit *>("makeLineEdit"));
    recommended_line_edits.append(parent()->findChild<QLineEdit *>("modelLineEdit"));

    auto recommended_combo_boxes = parent()->findChildren<QComboBox *>("operationComboBox");
    recommended_combo_boxes.append(parent()->findChild<QComboBox *>("ppNumberComboBox"));
    recommended_combo_boxes.append(parent()->findChild<QComboBox *>("ppTypeComboBox"));

    for (const auto le : recommended_line_edits) {
        if (le->text() != "") {
            DEB("Good: " << le);
            recommended_line_edits.removeOne(le);
            le->setStyleSheet("");
        } else {
            le->setStyleSheet("border: 1px solid red");
            DEB("Not Good: " << le);
        }
    }
    for (const auto cb : recommended_combo_boxes) {
        if (cb->currentIndex() != 0) {

            recommended_combo_boxes.removeOne(cb);
            cb->setStyleSheet("");
        } else {
            cb->setStyleSheet("background: orange");
            DEB("Not Good: " << cb);
        }
    }

    if (recommended_line_edits.isEmpty() && recommended_combo_boxes.isEmpty()) {
        return true;
    } else {
        return false;
    }
}

void NewTail::submitForm(db::editRole edRole)
{
    DEB("Creating Database Object...");
    QMap<QString, QString> newData;
    //retreive Line Edits
    auto line_edits = parent()->findChildren<QLineEdit *>();

    for (const auto &le : line_edits) {
        QString name = le->objectName();
        name.chop(8);//remove "LineEdit"
        if (!le->text().isEmpty()) {
            newData.insert(name, le->text());
        }
    }

    //prepare comboboxes
    QVector<QString> operation = {"singlepilot", "multipilot"};
    QVector<QString> ppNumber  = {"singleengine", "multiengine"};
    QVector<QString> ppType    = {"unpowered", "piston",
                                  "turboprop", "jet"
                                 };
    QVector<QString> weight    = {"light", "medium",
                                  "heavy", "super"
                                 };

    if (ui->operationComboBox->currentIndex() != 0) {
        newData.insert(operation[ui->operationComboBox->currentIndex() - 1], QLatin1String("1"));
    }
    if (ui->ppNumberComboBox->currentIndex() != 0) {
        newData.insert(ppNumber[ui->ppNumberComboBox->currentIndex() - 1], QLatin1String("1"));
    }
    if (ui->ppTypeComboBox->currentIndex() != 0) {
        newData.insert(ppType[ui->ppTypeComboBox->currentIndex() - 1], QLatin1String("1"));
    }
    if (ui->weightComboBox->currentIndex() != 0) {
        newData.insert(weight[ui->weightComboBox->currentIndex() - 1], QLatin1String("1"));
    }
    //create db object
    switch (edRole) {
    case db::createNew: {
        auto newEntry = new aircraft("tails", newData);;
        newEntry->commit();
        break;
    }
    case db::editExisting:
        oldEntry.setData(newData);
        oldEntry.commit();
        break;
    }
}

/// Slots

void NewTail::on_searchLineEdit_textChanged(const QString &arg1)
{
    if (aircraftlist.contains(
                arg1)) { //equivalent to editing finished for this purpose. todo: consider connecing qcompleter activated signal with editing finished slot.

        DEB("Template Selected. aircraft_id is: " << idMap.value(arg1));
        //call autofiller for dialog
        formFiller(aircraft("aircraft", idMap.value(arg1)));
        ui->searchLineEdit->setStyleSheet("border: 1px solid green");
    } else {
        //for example, editing finished without selecting a result from Qcompleter
        ui->searchLineEdit->setStyleSheet("border: 1px solid orange");
    }
}

void NewTail::on_operationComboBox_currentIndexChanged(int index)
{
    if (index != 0) {
        ui->operationComboBox->setStyleSheet("");
    }
}

void NewTail::on_ppTypeComboBox_currentIndexChanged(int index)
{
    if (index != 0) {
        ui->ppTypeComboBox->setStyleSheet("");
    }
}

void NewTail::on_ppNumberComboBox_currentIndexChanged(int index)
{
    if (index != 0) {
        ui->ppNumberComboBox->setStyleSheet("");
    }
}

void NewTail::on_weightComboBox_currentIndexChanged(int index)
{
    if (index != 0) {
        ui->weightComboBox->setStyleSheet("");
    }
}

void NewTail::on_buttonBox_accepted()
{
    DEB("Button Box Accepted.");
    if (ui->registrationLineEdit->text().isEmpty()) {
        auto nope = new QMessageBox(this);
        nope->setText("Registration cannot be empty.");
        nope->show();
    } else {
        if (verify()) {
            DEB("Form verified");
            submitForm(role);
            accept();
        } else {
            QSettings setting;
            if (!setting.value("userdata/acAllowIncomplete").toInt()) {
                auto nope = new QMessageBox(this);
                nope->setText("Some or all fields are empty.\nPlease go back and "
                              "complete.\n\nYou can allow logging incomplete entries on the settings page.");
                nope->show();
            } else {
                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(this, "Warning",
                                              "Some recommended fields are empty.\n\n"
                                              "If you do not fill out the aircraft details, "
                                              "it will be impossible to automatically determine Single/Multi Pilot Times or Single/Multi Engine Time."
                                              "This will also impact statistics and auto-logging capabilites.\n\n"
                                              "It is highly recommended to fill in all the details.\n\n"
                                              "Are you sure you want to proceed?",
                                              QMessageBox::Yes | QMessageBox::No);
                if (reply == QMessageBox::Yes) {
                    submitForm(role);
                    accept();
                }
            }
        }
    }
}
