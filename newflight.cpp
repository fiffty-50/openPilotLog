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
#include "newflight.h"
#include "ui_newflight.h"
#include "dbman.cpp"
#include <QMessageBox>
#include <QDebug>
#include <QCompleter>
#include <QLatin1Char>
#include <QStringList>
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QButtonGroup>

/*
 * Debug / WIP
 */
void NewFlight::on_verifyButton_clicked()//debug
{
    /*on_newDoft_editingFinished();// - activate slots in case user has been active in last input before clicking submit
    on_newTonb_editingFinished();
    on_newTofb_editingFinished();
    on_newDept_editingFinished();
    on_newDest_editingFinished();
    on_newAcft_editingFinished();
    on_newPic_editingFinished();
    verifyInput();*/
    fillExtrasLineEdits();

}
void NewFlight::on_tabWidget_tabBarClicked(int index)
{
    if(index == 1){
        nope();
    }
}
/*
 * Initialise variables
 */
QDate date(QDate::currentDate());
QString doft(QDate::currentDate().toString(Qt::ISODate));
QString dept;
QString dest;
QTime tofb;
QTime tonb;
QTime tblk;
QString pic;
QString acft;
QVector<QString> flight;
// extras
QString secondPilot;
QString thirdPilot;
QString PilotFunction;
QString PilotTask;
QString TakeOff;
QString Landing;
QString Autoland;
QString ApproachType;





/*
 * Window
 */

NewFlight::NewFlight(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewFlight)
{
    ui->setupUi(this);
    ui->newDoft->setDate(QDate::currentDate());

    bool hasoldinput = dbFlight::checkScratchpad();
    qDebug() << "Hasoldinput? = " << hasoldinput;
    if(hasoldinput) // Re-populate the Form
    {
        flight = dbFlight::retreiveScratchpad();
        qDebug() << "Re-Filling Form from Scratchpad";
        returnInput(flight);
    }

    // Validators for Line Edits
    QRegExp icao_rx("[a-zA-Z]?[a-zA-Z]?[a-zA-Z]?[a-zA-Z]"); // allow only letters (upper and lower)
    QValidator *ICAOvalidator = new QRegExpValidator(icao_rx, this);
    ui->newDept->setValidator(ICAOvalidator);
    ui->newDest->setValidator(ICAOvalidator);
    QRegExp timehhmm("([01]?[0-9]?|2[0-3]):?[0-5][0-9]?"); //allows time in 24h format with optional leading 0 and with or without seperator
    QValidator *timeInputValidator = new QRegExpValidator(timehhmm, this);
    ui->newTofb->setValidator(timeInputValidator);
    ui->newTonb->setValidator(timeInputValidator);
    ui->spseTimeLineEdit->setValidator(timeInputValidator);
    ui->spmeTimeLineEdit->setValidator(timeInputValidator);
    ui->mpTimeLineEdit->setValidator(timeInputValidator);
    ui->totalTimeLineEdit->setValidator(timeInputValidator);
    ui->ifrTimeLineEdit->setValidator(timeInputValidator);
    ui->vfrTimeLineEdit->setValidator(timeInputValidator);
    ui->nightTimeLineEdit->setValidator(timeInputValidator);
    ui->xcTimeLineEdit->setValidator(timeInputValidator);
    ui->picTimeLineEdit->setValidator(timeInputValidator);
    ui->copTimeLineEdit->setValidator(timeInputValidator);
    ui->dualTimeLineEdit->setValidator(timeInputValidator);
    ui->fiTimeLineEdit->setValidator(timeInputValidator);
    ui->simTimeLineEdit->setValidator(timeInputValidator);

    QRegExp picname("^[a-zA-Z_]+,?( [a-zA-Z_]+)*$");// allow only lastname, firstname or lastname firstname with one whitespace
    QValidator *picNameValidator = new QRegExpValidator(picname, this);
    ui->newPic->setValidator(picNameValidator);
    ui->secondPilotLineEdit->setValidator(picNameValidator);
    ui->thirdPilotLineEdit->setValidator(picNameValidator);

    // Groups for CheckBoxes
    QButtonGroup *FlightRulesGroup = new QButtonGroup(this);
    FlightRulesGroup->addButton(ui->IfrCheckBox);
    FlightRulesGroup->addButton(ui->VfrCheckBox);

    QButtonGroup *PilotTaskGroup = new QButtonGroup(this);
    PilotTaskGroup->addButton(ui->PilotFlyingCheckBox);
    PilotTaskGroup->addButton(ui->PilotMonitoringCheckBox);

    restoreSettings();
    ui->deptTZ->setFocusPolicy(Qt::NoFocus);
    ui->destTZ->setFocusPolicy(Qt::NoFocus);
    ui->newDept->setFocus();
}

NewFlight::~NewFlight()
{
    delete ui;
}

/*
 * Functions
 */

void NewFlight::nope()
{
    QMessageBox nope(this); //error box
    nope.setText("This feature is not yet available!");
    nope.exec();
}

/*!
 * \brief NewFlight::validateTimeInput verifies user input and formats to hh:mm
 * if the output is not a valid time, an empty string is returned.
 * \param userinput from a QLineEdit
 * \return formatted QString "hh:mm" or Empty String
 */
QString NewFlight::validateTimeInput(QString userinput)
{
    QString output; //
    QTime temptime; //empty time object is invalid by default

    bool containsSeperator = userinput.contains(":");
        if(userinput.length() == 4 && !containsSeperator)
        {
            temptime = QTime::fromString(userinput,"hhmm");
        }else if(userinput.length() == 3 && !containsSeperator)
        {
            if(userinput.toInt() < 240) //Qtime is invalid if time is between 000 and 240 for this case
            {
                QString tempstring = userinput.prepend("0");
                temptime = QTime::fromString(tempstring,"hhmm");
            }else
            {
                temptime = QTime::fromString(userinput,"Hmm");
            }
        }else if(userinput.length() == 4 && containsSeperator)
        {
            temptime = QTime::fromString(userinput,"h:mm");
        }else if(userinput.length() == 5 && containsSeperator)
        {
            temptime = QTime::fromString(userinput,"hh:mm");
        }

        output = temptime.toString("hh:mm");
        if(output.isEmpty())
        {
            QMessageBox timeformat(this);
            timeformat.setText("Please enter a valid time. Any of these formats is valid:\n845 0845 8:45 08:45");
            timeformat.exec();
        }
        return output;
}

/*!
 * \brief NewFlight::fillExtrasLineEdits Fills the flight time line edits according to ui selections
 */
void NewFlight::fillExtrasLineEdits()
{
    QString blockTime = calc::blocktime(tofb,tonb).toString("hh:mm");
    // SP SE
    // SP ME
    // MP
    // TOTAL
    ui->totalTimeLineEdit->setText(blockTime);
    // IFR
    // VFR
    // Night
    QString deptDate = date.toString(Qt::ISODate) + 'T' + tofb.toString("hh:mm");
    QDateTime deptDateTime = QDateTime::fromString(deptDate,"yyyy-MM-ddThh:mm");
    int blocktime = calc::time_to_minutes(calc::blocktime(tofb,tonb));
    qDebug() << "Blocktime: " << blocktime;
    QString nightTime = calc::minutes_to_string(QString::number(calc::calculateNightTime(dept, dest, deptDateTime, blocktime)));
    ui->nightTimeLineEdit->setText(nightTime);

    // XC - Cross-country flight, if more than 50nm long
    if(calc::greatCircleDistanceBetweenAirports(dept,dest) >= 50){
        qDebug() << "Cross-country Flight: nm = " << calc::greatCircleDistanceBetweenAirports(dept,dest);
        ui->xcTimeLineEdit->setText(blockTime);
    }else{
        ui->xcTimeLineEdit->setText("00:00");
    }
    // PIC
    // Co-Pilot
    // Dual
    // FI
    // SIM
}


QVector<QString> NewFlight::collectInput()
{
    // Collect input from the user fields and return a vector containing the flight information
    QString doft = date.toString(Qt::ISODate); // Format Date
    QTime tblk = calc::blocktime(tofb,tonb);   // Calculate Blocktime


    // Prepare Vector for commit to database
    flight = dbFlight::createFlightVectorFromInput(doft, dept, tofb, dest, tonb, tblk, pic, acft);
    qDebug() << "Created flight vector:" << flight;

    return flight;
}

bool NewFlight::verifyInput()
    //check if the input is correctly formatted and all required fields are filled
{

    bool deptValid = false;
    bool tofbValid = false;
    bool destValid = false;
    bool tonbValid = false;
    bool tblkValid = false;
    bool picValid = false;
    bool acftValid = false;

    QTime tblk = calc::blocktime(tofb,tonb);
    int checktblk = calc::time_to_minutes(tblk);

    bool doftValid = true; //doft assumed to be always valid due to QDateTimeEdit constraints
    qDebug() << "NewFlight::verifyInput() says: Date:" << doft << " - Valid?\t" << doftValid;

    deptValid = dbAirport::checkICAOValid(dept);
    qDebug() << "NewFlight::verifyInput() says: Departure is:\t" << dept << " - Valid?\t" << deptValid;

    destValid = dbAirport::checkICAOValid(dest);
    qDebug() << "NewFlight::verifyInput() says: Destination is:\t" << dest << " - Valid?\t" << destValid;

    tofbValid = (unsigned)(calc::time_to_minutes(tofb)-0) <= (1440-0) && tofb.toString("hh:mm") != ""; // Make sure time is within range, DB 1 day = 1440 minutes. 0 is allowed (midnight) & that it is not empty.
    qDebug() << "NewFlight::verifyInput() says: tofb is:\t\t" << tofb.toString("hh:mm") << " - Valid?\t" << tofbValid;

    tonbValid = (unsigned)(calc::time_to_minutes(tonb)-0) <= (1440-0) && tonb.toString("hh:mm") != ""; // Make sure time is within range, DB 1 day = 1440 minutes
    qDebug() << "NewFlight::verifyInput() says: tonb is:\t\t" << tonb.toString("hh:mm")<< " - Valid?\t" << tonbValid;

    picValid = (pic.toInt() > 0); // pic should be a pilot_id retreived from the database
    qDebug() << "NewFlight::verifyInput() says: pic is pilotd_id:\t" << pic << " - Valid?\t" << picValid;
    if(!picValid)
    {
        QMessageBox msgBox(this);
        msgBox.setText("You cannot log a flight without a valid pilot");
        msgBox.exec();
    }

    acftValid = (acft.toInt() > 0);
    qDebug() << "NewFlight::verifyInput() says: acft is tail_id:\t" << acft << " - Valid?\t" << acftValid;
    if(!acftValid)
    {
        QMessageBox msgBox(this);
        msgBox.setText("You cannot log a flight without a valid aircraft");
        msgBox.exec();
    }


    tblkValid = checktblk != 0;
    qDebug() << "NewFlight::verifyInput() says: tblk is:\t\t" << tblk.toString("hh:mm") << " - Valid?\t" << tblkValid;



    if(deptValid && tofbValid  && destValid && tonbValid
             && tblkValid && picValid && acftValid)
    {
        qDebug() << "====================================================";
        qDebug() << "NewFlight::verifyInput() says: All checks passed! Very impressive. ";
        return 1;
    }else
    {
        qDebug() << "====================================================";
        qDebug() << "NewFlight::verifyInput() says: Flight is invalid.";
        qDebug() << "NewFlight::verifyInput() says: I will call the cops.";
        return 0;
    }
    return 0;

}

void NewFlight::returnInput(QVector<QString> flight)
{
    // Re-populates the input masks with the selected fields if input was erroneous to allow for corrections to be made
    qDebug() << "return Input: " << flight;
    ui->newDoft->setDate(QDate::fromString(flight[1],Qt::ISODate));
    ui->newDept->setText(flight[2]);
    ui->newTofb->setText(flight[3]);
    ui->newDest->setText(flight[4]);
    ui->newTonb->setText(flight[5]);
    // flight[6] is blocktime
    ui->newPic->setText(dbPilots::retreivePilotNameFromID(flight[7]));
    ui->newAcft->setText(dbAircraft::retreiveRegistration(flight[8]));
}

void NewFlight::storeSettings()
{
    qDebug() << "Storing Settings...";
    dbSettings::storeSetting(100, ui->FunctionComboBox->currentText());
    dbSettings::storeSetting(101, ui->ApproachComboBox->currentText());
    dbSettings::storeSetting(102, QString::number(ui->PilotFlyingCheckBox->isChecked()));
    dbSettings::storeSetting(103, QString::number(ui->PilotMonitoringCheckBox->isChecked()));
    dbSettings::storeSetting(104, QString::number(ui->TakeoffSpinBox->value()));
    dbSettings::storeSetting(105, QString::number(ui->TakeoffCheckBox->isChecked()));
    dbSettings::storeSetting(106, QString::number(ui->LandingSpinBox->value()));
    dbSettings::storeSetting(107, QString::number(ui->LandingCheckBox->isChecked()));
    dbSettings::storeSetting(108, QString::number(ui->AutolandSpinBox->value()));
    dbSettings::storeSetting(109, QString::number(ui->AutolandCheckBox->isChecked()));
    dbSettings::storeSetting(110, QString::number(ui->IfrCheckBox->isChecked()));
    dbSettings::storeSetting(111, QString::number(ui->VfrCheckBox->isChecked()));
    //dbSettings::storesetting(112, QString::number(ui->autoNightCheckBox->isChecked()));
}
void NewFlight::restoreSettings()
{
    qDebug() << "Restoring Settings...";//crashes if db is empty due to QVector index out of range.
    ui->FunctionComboBox->setCurrentText(dbSettings::retreiveSetting(100));
    ui->ApproachComboBox->setCurrentText(dbSettings::retreiveSetting(101));
    ui->PilotFlyingCheckBox->setChecked(dbSettings::retreiveSetting(102).toInt());
    ui->PilotMonitoringCheckBox->setChecked(dbSettings::retreiveSetting(103).toInt());
    ui->TakeoffSpinBox->setValue(dbSettings::retreiveSetting(104).toInt());
    ui->TakeoffCheckBox->setChecked(dbSettings::retreiveSetting(105).toInt());
    ui->LandingSpinBox->setValue(dbSettings::retreiveSetting(106).toInt());
    ui->LandingCheckBox->setChecked(dbSettings::retreiveSetting(107).toInt());
    ui->AutolandSpinBox->setValue(dbSettings::retreiveSetting(108).toInt());
    ui->AutolandCheckBox->setChecked(dbSettings::retreiveSetting(109).toInt());
    ui->IfrCheckBox->setChecked(dbSettings::retreiveSetting(110).toInt());
    ui->VfrCheckBox->setChecked(dbSettings::retreiveSetting(111).toInt());

    ui->flightNumberPrefixLabel->setText(dbSettings::retreiveSetting(50) + QLatin1Char('-'));
    //ui->autoNightCheckBox->setChecked(dbSettings::retreiveSetting("112")[1].toInt());
    //qDebug() << "restore Settings ifr to int: " << dbSettings::retreiveSetting("110")[1].toInt();

/*
 *
 * QString PilotFunction;
QString PilotTask;
QString TakeOff;
QString Landing;
QString Autoland;
QString ApproachType;
100	PIC	NewFlight::FunctionComboBox
101	ILS CAT I	NewFlight::ApproachComboBox
102	Qt::Checked	NewFlight::PilotFlyingCheckBox
103	Qt::Unchecked	NewFlight::PilotMonitoringCheckBox
104	1	NewFlight::TakeoffSpinBox
105	Qt::Checked	NewFlight::TakeoffCheckBox
106	1	NewFlight::LandingSpinBox
107	Qt::Checked	NewFlight::LandingCheckBox
108	0	NewFlight::AutolandSpinBox
109	Qt::Unchecked	NewFlight::AutolandCheckBox
110	Qt::Checked	NewFlight::IfrCheckBox
111	Qt::Unchecked	NewFlight::VfrCheckBox
*/
}
/*
 * Slots
 */



void NewFlight::on_deptTZ_currentTextChanged(const QString &arg1)
{
    if(arg1 == "Local"){nope();}
    ui->deptTZ->setCurrentIndex(0);
}

void NewFlight::on_destTZ_currentIndexChanged(const QString &arg1)
{
    if(arg1 == "Local"){nope();}
    ui->destTZ->setCurrentIndex(0);
}


void NewFlight::on_newDept_textEdited(const QString &arg1)
{
    ui->newDept->setText(arg1.toUpper());
    if(arg1.length()>2)
    {
        QStringList deptList = dbAirport::completeIcaoOrIata(arg1);
        qDebug() << "deptList = " << deptList;
        QCompleter *deptCompleter = new QCompleter(deptList, this);
        deptCompleter->setCaseSensitivity(Qt::CaseInsensitive);
        deptCompleter->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
        ui->newDept->setCompleter(deptCompleter);
    }
}

void NewFlight::on_newDept_editingFinished()
{
    if(ui->newDept->text().length()<3)
    {
        QMessageBox msgBox(this);
        msgBox.setText("Please enter a 3- or 4-letter code.");
        msgBox.exec();
        ui->newDept->setText("");
        ui->newDept->setFocus();
    }

    QStringList deptList;

    if(ui->newDept->text().length()>1)
    {
        QStringList deptList = dbAirport::completeIcaoOrIata(ui->newDept->text());
        if(deptList.length() != 0)
        {
            dept = deptList.first();
            ui->newDept->setText(dept);

        }else
        {
            QMessageBox msgBox(this);
            msgBox.setText("No Airport with that name found.");
            msgBox.exec();
            ui->newDept->setText("");
            ui->newDept->setFocus();
        }
    }



}

void NewFlight::on_newDest_textEdited(const QString &arg1)
{
    ui->newDest->setText(arg1.toUpper());
    if(arg1.length()>2)
    {
        QStringList destList = dbAirport::completeIcaoOrIata(arg1);
        QCompleter *destCompleter = new QCompleter(destList, this);
        destCompleter->setCaseSensitivity(Qt::CaseInsensitive);
        destCompleter->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
        ui->newDest->setCompleter(destCompleter);
    }

}

void NewFlight::on_newDest_editingFinished()
{
    if(ui->newDest->text().length()<3)
    {
        QMessageBox msgBox(this);
        msgBox.setText("Please enter a 3- or 4-letter code.");
        msgBox.exec();
        ui->newDest->setText("");
        ui->newDest->setFocus();
    }
    QStringList destList;
    if(ui->newDest->text().length()>1)
    {
        QStringList destList = dbAirport::completeIcaoOrIata(ui->newDest->text());
        if(destList.length() != 0)
        {
            dest = destList.first();
            ui->newDest->setText(dest);
        }else
        {
            QMessageBox msgBox(this);
            msgBox.setText("No Airport with that name found.");
            msgBox.exec();
            ui->newDest->setText("");
            ui->newDest->setFocus();
        }
    }
}


void NewFlight::on_newDoft_editingFinished()
{
    date = ui->newDoft->date();
    doft = date.toString(Qt::ISODate);
}

void NewFlight::on_newTofb_editingFinished()
{
    ui->newTofb->setText(validateTimeInput(ui->newTofb->text()));
    tofb = QTime::fromString(ui->newTofb->text(),"hh:mm");

    if(!tofb.isValid()){
        ui->newTofb->setStyleSheet("border: 1px solid red");
        ui->newTofb->setFocus();
    }else{
        ui->newTofb->setStyleSheet("");
    }
    qDebug() << "New Time Off Blocks: " << tofb;
}

void NewFlight::on_newTonb_editingFinished()
{
    ui->newTonb->setText(validateTimeInput(ui->newTonb->text()));
    tonb = QTime::fromString(ui->newTonb->text(),"hh:mm");

    if(!tonb.isValid()){
        ui->newTonb->setStyleSheet("border: 1px solid red");
        ui->newTonb->setFocus();
    }else{
        ui->newTonb->setStyleSheet("");
    }
    qDebug() << "New Time On Blocks: " << tonb;
}


void NewFlight::on_newAcft_textEdited(const QString &arg1)
{
    if(arg1.length()>1)
    {
        QStringList acftList = dbAircraft::newAcftGetString(arg1);
        QCompleter *acftCompleter = new QCompleter(acftList, this);
        acftCompleter->setCaseSensitivity(Qt::CaseInsensitive);
        acftCompleter->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
        ui->newAcft->setCompleter(acftCompleter);
     }
}

void NewFlight::on_newAcft_editingFinished()
{
    acft = "-1";// set invalid

    if(ui->newAcft->text().contains(" "))// is input from QCompleter
    {
        QStringList input = ui->newAcft->text().split(" ");
        QString registration = input[0].trimmed();
        QStringList result = dbAircraft::newAcftGetString(registration);
        if(result.length() > 0)
        {
            ui->newAcft->setText(result[0]);
            acft = dbAircraft::newAcftGetId(registration);
        }else
        {
            acft = "-1";
        }
    }else // is input from user
    {
        QString input = ui->newAcft->text();
        QStringList result = dbAircraft::newAcftGetString(input);
        if(result.length() > 0)
        {
            ui->newAcft->setText(result[0]);

            QStringList input = ui->newAcft->text().split(" ");
            QString registration = input[0].trimmed();
            acft = dbAircraft::newAcftGetId(registration);
        }else
        {
            acft = "-1";
        }
    }


    if(acft == "-1")
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "No aircraft found", "No aircraft found.\n Would you like to add a new aircraft to the database?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            NewAcft na(this);
            na.exec();
            ui->newAcft->setText("");
            ui->newAcft->setFocus();
        }

    }

    qDebug() << "Editing finished. Acft: " << acft;
}


void NewFlight::on_newPic_textEdited(const QString &arg1)
{
    if(arg1.length()>2)
    {
        QStringList picList = dbPilots::newPicGetString(arg1);
        QCompleter *picCompleter = new QCompleter(picList, this);
        picCompleter->setCaseSensitivity(Qt::CaseInsensitive);
        picCompleter->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
        ui->newPic->setCompleter(picCompleter);
     }
}
void NewFlight::on_newPic_editingFinished()
{
    pic = "-1"; // set invalid
    if(ui->newPic->text() == "self")
    {
        pic = "1";
    }else
    {
        QString picname;
        QStringList picList = dbPilots::newPicGetString(ui->newPic->text());
        qDebug() << picList;
        if(picList.length()!= 0)
        {
            picname = picList[0];
            ui->newPic->setText(picname);
            pic = dbPilots::newPicGetId(picname);
        }else
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "No Pilot found", "No pilot found.\n Would you like to add a new pilot to the database?",
                                          QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes)
            {
                qDebug() << "Add new pilot selected";
                nope();
            }
        }
    }
}

void NewFlight::on_secondPilotLineEdit_textEdited(const QString &arg1)
{
    if(arg1.length()>2)
    {
        QStringList picList = dbPilots::newPicGetString(arg1);
        QCompleter *picCompleter = new QCompleter(picList, this);
        picCompleter->setCaseSensitivity(Qt::CaseInsensitive);
        picCompleter->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
        ui->secondPilotLineEdit->setCompleter(picCompleter);
     }
}

void NewFlight::on_secondPilotLineEdit_editingFinished()
{
    secondPilot = "-1"; // set invalid
    if(ui->secondPilotLineEdit->text() == "self")
    {
        secondPilot = "1";
    }else
    {
        QString picname;
        QStringList picList = dbPilots::newPicGetString(ui->secondPilotLineEdit->text());
        qDebug() << picList;
        if(picList.length()!= 0)
        {
            picname = picList[0];
            ui->secondPilotLineEdit->setText(picname);
            secondPilot = dbPilots::newPicGetId(picname);
        }else
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "No Pilot found", "No pilot found.\n Would you like to add a new pilot to the database?",
                                          QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes)
            {
                qDebug() << "Add new pilot selected";
                nope();
            }
        }
    }
}

void NewFlight::on_thirdPilotLineEdit_textEdited(const QString &arg1)
{
    if(arg1.length()>2)
    {
        QStringList picList = dbPilots::newPicGetString(arg1);
        QCompleter *picCompleter = new QCompleter(picList, this);
        picCompleter->setCaseSensitivity(Qt::CaseInsensitive);
        picCompleter->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
        ui->thirdPilotLineEdit->setCompleter(picCompleter);
     }
}

void NewFlight::on_thirdPilotLineEdit_editingFinished()
{
    thirdPilot = "-1"; // set invalid
    if(ui->thirdPilotLineEdit->text() == "self")
    {
        thirdPilot = "1";
    }else
    {
        QString picname;
        QStringList picList = dbPilots::newPicGetString(ui->thirdPilotLineEdit->text());
        qDebug() << picList;
        if(picList.length()!= 0)
        {
            picname = picList[0];
            ui->thirdPilotLineEdit->setText(picname);
            thirdPilot = dbPilots::newPicGetId(picname);
        }else
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "No Pilot found", "No pilot found.\n Would you like to add a new pilot to the database?",
                                          QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes)
            {
                qDebug() << "Add new pilot selected";
                nope();
            }
        }
    }
}

void NewFlight::on_FlightNumberLineEdit_editingFinished()
{
    qDebug() << "tbd: FlightNumber slot";
    // To Do: Store input in variable, perform some checks
    // Setting for optional Prefix (e.g. LH, LX etc.)
}

/*
 * Extras
 */

void NewFlight::on_PilotFlyingCheckBox_stateChanged(int)
{
    if(ui->PilotFlyingCheckBox->isChecked()){
        ui->TakeoffSpinBox->setValue(1);
        ui->TakeoffCheckBox->setCheckState(Qt::Checked);
        ui->LandingSpinBox->setValue(1);
        ui->LandingCheckBox->setCheckState(Qt::Checked);

    }else if(!ui->PilotFlyingCheckBox->isChecked()){
        ui->TakeoffSpinBox->setValue(0);
        ui->TakeoffCheckBox->setCheckState(Qt::Unchecked);
        ui->LandingSpinBox->setValue(0);
        ui->LandingCheckBox->setCheckState(Qt::Unchecked);
    }
}



void NewFlight::on_setAsDefaultButton_clicked()
{
    storeSettings();
}

void NewFlight::on_restoreDefaultButton_clicked()
{
    restoreSettings();
}

void NewFlight::on_ApproachComboBox_currentTextChanged(const QString &arg1)
{
    if(arg1 == "ILS CAT III"){
        ui->AutolandCheckBox->setCheckState(Qt::Checked);
        ui->AutolandSpinBox->setValue(1);
    }else{
        ui->AutolandCheckBox->setCheckState(Qt::Unchecked);
        ui->AutolandSpinBox->setValue(0);
    }
    ApproachType = arg1;
    qDebug() << "Approach Type: " << ApproachType;
}

/*
 * Times
 */

void NewFlight::on_spseTimeLineEdit_editingFinished()
{
    ui->spseTimeLineEdit->setText(validateTimeInput(ui->spseTimeLineEdit->text()));
    if(ui->spseTimeLineEdit->text() == ""){
        ui->spseTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->spseTimeLineEdit->setFocus();
    }else{
        ui->spseTimeLineEdit->setStyleSheet("");
    }
}

void NewFlight::on_spmeTimeLineEdit_editingFinished()
{
    ui->spmeTimeLineEdit->setText(validateTimeInput(ui->spmeTimeLineEdit->text()));
    if(ui->spmeTimeLineEdit->text() == ""){
        ui->spmeTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->spmeTimeLineEdit->setFocus();
    }else{
        ui->spmeTimeLineEdit->setStyleSheet("");
    }
}

void NewFlight::on_mpTimeLineEdit_editingFinished()
{
    ui->mpTimeLineEdit->setText(validateTimeInput(ui->mpTimeLineEdit->text()));
    if(ui->mpTimeLineEdit->text() == ""){
        ui->mpTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->mpTimeLineEdit->setFocus();
    }else{
        ui->mpTimeLineEdit->setStyleSheet("");
    }
}

void NewFlight::on_totalTimeLineEdit_editingFinished()
{
    ui->totalTimeLineEdit->setText(validateTimeInput(ui->totalTimeLineEdit->text()));
    if(ui->totalTimeLineEdit->text() == ""){
        ui->totalTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->totalTimeLineEdit->setFocus();
    }else{
        ui->totalTimeLineEdit->setStyleSheet("");
    }
}

void NewFlight::on_ifrTimeLineEdit_editingFinished()
{
    ui->ifrTimeLineEdit->setText(validateTimeInput(ui->ifrTimeLineEdit->text()));
    if(ui->ifrTimeLineEdit->text() == ""){
        ui->ifrTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->ifrTimeLineEdit->setFocus();
    }else{
        ui->ifrTimeLineEdit->setStyleSheet("");
    }
}

void NewFlight::on_vfrTimeLineEdit_editingFinished()
{
    ui->vfrTimeLineEdit->setText(validateTimeInput(ui->vfrTimeLineEdit->text()));
    if(ui->vfrTimeLineEdit->text() == ""){
        ui->vfrTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->vfrTimeLineEdit->setFocus();
    }else{
        ui->vfrTimeLineEdit->setStyleSheet("");
    }
}

void NewFlight::on_nightTimeLineEdit_editingFinished()
{
    ui->nightTimeLineEdit->setText(validateTimeInput(ui->nightTimeLineEdit->text()));
    if(ui->nightTimeLineEdit->text() == ""){
        ui->nightTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->nightTimeLineEdit->setFocus();
    }else{
        ui->nightTimeLineEdit->setStyleSheet("");
    }
}

void NewFlight::on_xcTimeLineEdit_editingFinished()
{
    ui->xcTimeLineEdit->setText(validateTimeInput(ui->xcTimeLineEdit->text()));
    if(ui->xcTimeLineEdit->text() == ""){
        ui->xcTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->xcTimeLineEdit->setFocus();
    }else{
        ui->xcTimeLineEdit->setStyleSheet("");
    }
}

void NewFlight::on_picTimeLineEdit_editingFinished()
{
    ui->picTimeLineEdit->setText(validateTimeInput(ui->picTimeLineEdit->text()));
    if(ui->picTimeLineEdit->text() == ""){
        ui->picTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->picTimeLineEdit->setFocus();
    }else{
        ui->picTimeLineEdit->setStyleSheet("");
    }
}

void NewFlight::on_copTimeLineEdit_editingFinished()
{
    ui->copTimeLineEdit->setText(validateTimeInput(ui->copTimeLineEdit->text()));
    if(ui->copTimeLineEdit->text() == ""){
        ui->copTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->copTimeLineEdit->setFocus();
    }else{
        ui->copTimeLineEdit->setStyleSheet("");
    }
}

void NewFlight::on_dualTimeLineEdit_editingFinished()
{
    ui->dualTimeLineEdit->setText(validateTimeInput(ui->dualTimeLineEdit->text()));
    if(ui->dualTimeLineEdit->text() == ""){
        ui->dualTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->dualTimeLineEdit->setFocus();
    }else{
        ui->dualTimeLineEdit->setStyleSheet("");
    }
}

void NewFlight::on_fiTimeLineEdit_editingFinished()
{
    ui->fiTimeLineEdit->setText(validateTimeInput(ui->fiTimeLineEdit->text()));
    if(ui->fiTimeLineEdit->text() == ""){
        ui->fiTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->fiTimeLineEdit->setFocus();
    }else{
        ui->fiTimeLineEdit->setStyleSheet("");
    }
}

void NewFlight::on_simTimeLineEdit_editingFinished()
{
    ui->simTimeLineEdit->setText(validateTimeInput(ui->simTimeLineEdit->text()));
    if(ui->simTimeLineEdit->text() == ""){
        ui->simTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->simTimeLineEdit->setFocus();
    }else{
        ui->simTimeLineEdit->setStyleSheet("");
    }
}


/*
 * Control Buttons
 */

void NewFlight::on_buttonBox_accepted()
{
    on_newDoft_editingFinished();// - activate slots in case user has been active in last input before clicking submit
    on_newTonb_editingFinished();
    on_newTofb_editingFinished();
    on_newDept_editingFinished();
    on_newDest_editingFinished();
    on_newAcft_editingFinished();
    on_newPic_editingFinished();

        QVector<QString> flight;
        flight = collectInput();
        if(verifyInput())
        {
            dbFlight::commitFlight(flight);
            qDebug() << flight << "Has been commited.";
            QMessageBox msgBox(this);
            msgBox.setText("Flight has been commited.");
            msgBox.exec();
        }else
        {
            qDebug() << "Invalid Input. No entry has been made in the database.";
            dbFlight::commitToScratchpad(flight);
            QMessageBox msgBox(this);
            msgBox.setText("Invalid entries detected. Please check your input.");
            msgBox.exec();
            NewFlight nf(this);
            nf.exec();
        }
}

void NewFlight::on_buttonBox_rejected()
{
    qDebug() << "NewFlight: Rejected\n";
}
