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
#include "editflight.h"
#include "ui_editflight.h"
#include "calc.h"
#include "dbman.cpp"
#include "dbflight.h"
#include <QMessageBox>
#include <QDebug>

/*
 * Initialise variables
 */
QDate editdate(QDate::currentDate());
QString editdoft(QDate::currentDate().toString(Qt::ISODate));
QString editdept;
QString editdest;
QTime edittofb;
QTime edittonb;
QTime edittblk;
QString editpic;
QString editacft;
QVector<QString> editflight;






/*
 * Window
 */

EditFlight::EditFlight(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditFlight)
{
    ui->setupUi(this);

    editflight = dbFlight::retreiveScratchpad();
    qDebug() << "EditFlight: Re-assigning variables from vector " << editflight;

    editdate = QDate::fromString(editflight[1],Qt::ISODate);
    editdoft = editflight[1];
    editdept = editflight[2];
    edittofb = QTime::fromString(editflight[3], "hh:mm");
    editdest = editflight[4];
    edittonb = QTime::fromString(editflight[5], "hh:mm");
    // flight[6] is blocktime
    editpic = editflight[7];
    editacft = editflight[8];
    qDebug() << "Reassigned:" << editdate << editdoft << editdept << edittofb << editdest << edittonb << editpic << editacft;

    qDebug() << "Re-Filling Form from Scratchpad";
    returnInput(editflight);



    // Validators for Line Edits
    QRegExp icao_rx("[a-zA-Z]?[a-zA-Z]?[a-zA-Z]?[a-zA-Z]"); // allow only letters (upper and lower)
    QValidator *ICAOvalidator = new QRegExpValidator(icao_rx, this);
    ui->newDept->setValidator(ICAOvalidator);
    ui->newDest->setValidator(ICAOvalidator);
    QRegExp timehhmm("([01]?[0-9]|2[0-3]):?[0-5][0-9]"); //allows time in 24h format with optional leading 0 and with or without seperator
    QValidator *timeInputValidator = new QRegExpValidator(timehhmm, this);
    ui->newTofb->setValidator(timeInputValidator);
    ui->newTonb->setValidator(timeInputValidator);
    ui->deptHintlineEdit->setFocusPolicy(Qt::NoFocus);
    ui->destHintlineEdit->setFocusPolicy(Qt::NoFocus);
    ui->acftHintLineEdit->setFocusPolicy(Qt::NoFocus);
    ui->picHintLineEdit->setFocusPolicy(Qt::NoFocus);
    ui->deptTZ->setFocusPolicy(Qt::NoFocus);
    ui->desTZ->setFocusPolicy(Qt::NoFocus);
    ui->newDept->setFocus();

}

EditFlight::~EditFlight()
{
    delete ui;
}

/*
 * Functions
 */

void EditFlight::nope()
{
    QMessageBox nope; //error box
    nope.setText("This feature is not yet available!");
    nope.exec();
}


QVector<QString> EditFlight::collectInput()
{
    // Collect input from the user fields and return a vector containing the flight information
    QString editdoft = editdate.toString(Qt::ISODate); // Format Date
    QTime tblk = calc::blocktime(edittofb,edittonb);   // Calculate Blocktime


    // Prepare Vector for commit to database
    editflight = dbFlight::createFlightVectorFromInput(editdoft, editdept, edittofb, editdest, edittonb, tblk, editpic, editacft);
    qDebug() << "Created flight vector:" << editflight;

    return editflight;
}

bool EditFlight::verifyInput()
    //check if the input is correctly formatted and all required fields are filled
{

    bool deptValid = false;
    bool tofbValid = false;
    bool destValid = false;
    bool tonbValid = false;
    bool tblkValid = false;
    bool picValid = false;
    bool acftValid = false;

    QTime tblk = calc::blocktime(edittofb,edittonb);
    int checktblk = calc::time_to_minutes(tblk);

    bool doftValid = true; //doft assumed to be always valid due to QDateTimeEdit constraints
    qDebug() << "EditFlight::verifyInput() says: Date:" << editdoft << " - Valid?\t" << doftValid;

    deptValid = dbAirport::checkICAOValid(editdept);
    qDebug() << "EditFlight::verifyInput() says: Departure is:\t" << editdept << " - Valid?\t" << deptValid;

    destValid = dbAirport::checkICAOValid(editdest);
    qDebug() << "EditFlight::verifyInput() says: Destination is:\t" << editdest << " - Valid?\t" << destValid;

    tofbValid = (unsigned)(calc::time_to_minutes(edittofb)-0) <= (1440-0) && edittofb.toString("hh:mm") != ""; // Make sure time is within range, DB 1 day = 1440 minutes. 0 is allowed (midnight) & that it is not empty.
    qDebug() << "EditFlight::verifyInput() says: tofb is:\t\t" << edittofb.toString("hh:mm") << " - Valid?\t" << tofbValid;

    tonbValid = (unsigned)(calc::time_to_minutes(edittonb)-0) <= (1440-0) && edittofb.toString("hh:mm") != ""; // Make sure time is within range, DB 1 day = 1440 minutes
    qDebug() << "EditFlight::verifyInput() says: tonb is:\t\t" << edittonb.toString("hh:mm")<< " - Valid?\t" << tonbValid;

    picValid = (editpic.toInt() != 0); // pic should be a pilot_id retreived from the database
    qDebug() << "EditFlight::verifyInput() says: pic is pilotd_id:\t" << editpic << " - Valid?\t" << picValid;

    acftValid = (editacft.toInt() != 0);
    qDebug() << "EditFlight::verifyInput() says: acft is tail_id:\t" << editacft << " - Valid?\t" << acftValid;

    tblkValid = checktblk != 0;
    qDebug() << "EditFlight::verifyInput() says: tblk is:\t\t" << tblk.toString("hh:mm") << " - Valid?\t" << tblkValid;



    if(deptValid && tofbValid  && destValid && tonbValid
             && tblkValid && picValid && acftValid)
    {
        qDebug() << "====================================================";
        qDebug() << "EditFlight::verifyInput() says: All checks passed! Very impressive. ";
        return 1;
    }else
    {
        qDebug() << "====================================================";
        qDebug() << "EditFlight::verifyInput() says: Flight is invalid.";
        qDebug() << "EditFlight::verifyInput() says: I will call the cops.";
        return 0;
    }
    return 0;

}

void EditFlight::returnInput(QVector<QString> flight)
{
    // Re-populates the input masks with the selected fields if input was erroneous to allow for corrections to be made
    ui->newDoft->setDate(QDate::fromString(flight[1],Qt::ISODate));
    ui->newDept->setText(flight[2]);
    ui->newTofb->setText(flight[3]);
    ui->newDest->setText(flight[4]);
    ui->newTonb->setText(flight[5]);
    // flight[6] is blocktime
    ui->newPic->setText(dbPilots::retreivePilotNameFromID(flight[7]));
    ui->newAcft->setText(db::RetreiveRegistration(flight[8]));
}

/*
 * Slots
 */

void EditFlight::on_newDoft_editingFinished()
{
    editdate = ui->newDoft->date();
    editdoft = editdate.toString(Qt::ISODate);
    //ui->dateHintLineEdit->setText(doft);
}

void EditFlight::on_newDept_textChanged(const QString &arg1)
{
    if(arg1.length() > 2)
    {
        QString result;
        result = dbAirport::retreiveAirportNameFromIcaoOrIata(arg1);
        ui->deptHintlineEdit->setPlaceholderText(result);
    }
}

void EditFlight::on_newDest_textChanged(const QString &arg1)
{
    if(arg1.length() > 2)
    {
        QString result;
        result = dbAirport::retreiveAirportNameFromIcaoOrIata(arg1);
        ui->destHintlineEdit->setPlaceholderText(result);
    }
}

void EditFlight::on_newDept_editingFinished()
{
    editdept = ui->newDept->text().toUpper();
    ui->newDept->setText(editdept);
    if (!ui->deptHintlineEdit->placeholderText().compare("No matching airport found.", Qt::CaseInsensitive))
    {
        QMessageBox msgBox;
        msgBox.setText("Airport not found.\nCreate new entry in Database?");
        msgBox.exec();
    }else if (editdept.length() < 4)
    {
        qDebug() << "on_new_Dept_editingFinished() Incomplete entry. Completing.";
        //editdept = db::CompleteIcaoOrIata(editdept);
        ui->newDept->setText(editdept);
    }

}

void EditFlight::on_newTofb_editingFinished()
{
    bool containsSeperator = ui->newTofb->text().contains(":");
    if(ui->newTofb->text().length() == 4 && !containsSeperator)
    {
        qDebug() << "1) Contains seperator: " << containsSeperator << "Length = " << ui->newTofb->text().length();
        edittofb = QTime::fromString(ui->newTofb->text(),"hhmm");
        qDebug() << edittofb;
    }else if(ui->newTofb->text().length() == 3 && !containsSeperator)
    {
        if(ui->newTofb->text().toInt() < 240) //Qtime is invalid if time is between 000 and 240 for this case
        {
            QString tempstring = ui->newTofb->text().prepend("0");
            edittofb = QTime::fromString(tempstring,"hhmm");
            qDebug() << tempstring << "is the tempstring (Special Case) " << edittofb;
        }else
        {
            qDebug() << "2) Contains seperator: " << containsSeperator << "Length = " << ui->newTofb->text().length();
            qDebug() << "Tofb = " << ui->newTofb->text();
            edittofb = QTime::fromString(ui->newTofb->text(),"Hmm");
            qDebug() << edittofb;
        }
    }else if(ui->newTofb->text().length() == 4 && containsSeperator)
    {
        qDebug() << "3) Contains seperator: " << containsSeperator << "Length = " << ui->newTofb->text().length();
        edittofb = QTime::fromString(ui->newTofb->text(),"h:mm");
        qDebug() << edittofb;

    }else if(ui->newTofb->text().length() == 5 && containsSeperator)
    {
        qDebug() << "4) Contains seperator: " << containsSeperator << "Length = " << ui->newTofb->text().length();
        edittofb = QTime::fromString(ui->newTofb->text(),"hh:mm");
        qDebug() << edittofb;
    }
    ui->newTofb->setText(edittofb.toString("hh:mm"));
}

void EditFlight::on_newDest_editingFinished()
{
    editdest = ui->newDest->text().toUpper();
    ui->newDest->setText(editdest);
    if (!ui->destHintlineEdit->placeholderText().compare("No matching airport found.", Qt::CaseInsensitive))
    {
        QMessageBox msgBox;
        msgBox.setText("Airport not found.\nCreate new entry in Database?");
        msgBox.exec();
    }else if (editdest.length() < 4)
    {
        qDebug() << "on_new_Dest_editingFinished() Incomplete entry. Completing.";
        //editdest = db::CompleteIcaoOrIata(editdest);
        ui->newDest->setText(editdest);
        //ui->newDest->setText(db::CompleteIcaoOrIata(dest));
    }
}

void EditFlight::on_newTonb_editingFinished()
{
    bool containsSeperator = ui->newTonb->text().contains(":");
    if(ui->newTonb->text().length() == 4 && !containsSeperator)
    {
        edittonb = QTime::fromString(ui->newTonb->text(),"hhmm");
        qDebug() << edittonb;
    }else if(ui->newTonb->text().length() == 3 && !containsSeperator)
    {
        if(ui->newTonb->text().toInt() < 240) //Qtime is invalid if time is between 000 and 240 for this case
        {
            QString tempstring = ui->newTonb->text().prepend("0");
            edittonb = QTime::fromString(tempstring,"hhmm");
            qDebug() << tempstring << "is the tempstring (Special Case) " << edittonb;
        }else
        {
            qDebug() << "Tofb = " << ui->newTonb->text();
            edittonb = QTime::fromString(ui->newTonb->text(),"Hmm");
            qDebug() << edittonb;
        }
    }else if(ui->newTonb->text().length() == 4 && containsSeperator)
    {
        edittonb = QTime::fromString(ui->newTonb->text(),"h:mm");
        qDebug() << edittonb;

    }else if(ui->newTonb->text().length() == 5 && containsSeperator)
    {
        edittonb = QTime::fromString(ui->newTonb->text(),"hh:mm");
        qDebug() << edittonb;
    }
    ui->newTonb->setText(edittonb.toString("hh:mm"));
}

void EditFlight::on_newPic_textChanged(const QString &arg1)
{
    qDebug() << arg1;
    /*{
        if(arg1.length() > 3)
        {
            QVector<QString> hint = db::RetreivePilotNameFromString(arg1);
            if(hint.size()!= 0)
            {
                QString combinedname = hint[0] + ", " + hint[1] + " [ " + hint[2] + " ]";
                ui->picHintLineEdit->setPlaceholderText(combinedname);
            }
        }
    }*/

}
void EditFlight::on_newPic_editingFinished()
{
    /*QString input = ui->newPic->text();
    if(input.length()>2)
    {
        QVector<QString> result;
        result = db::RetreivePilotNameFromString(input);
        qDebug() << result;
        if(result.size()!=0)
        {
            QString lastname = result[0];
            ui->newPic->setText(lastname);// to do: pop up to navigate result set, here first result is selected

            editpic = db::RetreivePilotIdFromString(lastname);// to do, adjust before commit
            //qDebug() << "on_newPic_editingFinished() says: Pilot ID = " << pic;
        }else
        {
            QMessageBox msgBox;
            msgBox.setText("No Pilot found!");// to do: add new pilot
            msgBox.exec();
            ui->newPic->setText("");
            ui->newPic->setFocus();
        }

    }else if (input.length() == 0)
    {

    }else
    {
        QMessageBox msgBox;
        msgBox.setText("For Auto-Completion, please enter 3 or more characters!");
        msgBox.exec();
        ui->newPic->setFocus();
    }*/

}

void EditFlight::on_newAcft_textChanged(const QString &arg1)
{
    if(arg1.length() > 2)
    {
        QVector<QString> hint = db::RetreiveAircraftTypeFromReg(arg1);
        if(hint.size() != 0)
        {
            ui->acftHintLineEdit->setPlaceholderText(hint[1] + " [ " + hint[0] + " | " + hint[2] + " ]");
        }

    }
}

void EditFlight::on_newAcft_editingFinished()
{
    editacft = ui->newAcft->text();
    if(ui->newAcft->text().length()>2)
    {
        QVector<QString> result;
        result = db::RetreiveAircraftTypeFromReg(editacft);
        if(result.size()!=0)
        {
            ui->newAcft->setText(result[0]);// to do: pop up to navigate result set, here first result is selected
            editacft = result[3];
        }else
        {
            QMessageBox msgBox;
            msgBox.setText("No Aircraft found!");
            msgBox.exec();
            ui->newAcft->setText("");
            ui->newAcft->setFocus();
        }
    }
}


void EditFlight::on_buttonBox_accepted()
{
    QVector<QString> flight;
    flight = collectInput();
    if(verifyInput())
    {
        //!x db::CommitFlight(flight);
        qDebug() << flight << "Has been commited.";
        QMessageBox msgBox;
        msgBox.setText("Flight successfully edited. (At least once I wrote that function. Now nothing has happened.)\nlol\nWhat a troll.");
        msgBox.exec();
    }else
    {
        qDebug() << "Invalid Input. No entry has been made in the database.";
        dbFlight::commitToScratchpad(flight);
        QMessageBox msgBox;
        msgBox.setText("La Base de datos se niega a ser violada!");
        msgBox.exec();
        EditFlight nf(this);
        nf.exec();
    }

}

void EditFlight::on_buttonBox_rejected()
{
    dbFlight::clearScratchpad();
}

void EditFlight::on_verifyButton_clicked()
{
    editflight = collectInput();
    if (verifyInput())
    {
        ui->verifyEdit->setPlaceholderText("No Errors detected.");
    }else
    {
    ui->verifyEdit->setPlaceholderText("Invalid Input.");
    }
}
