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
    qDebug() << "NewTail ::" << __func__ << "\t" << expr

//Dialog to be used to edit existing tail
NewTail::NewTail(aircraft acft, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewTail)
{
    ui->setupUi(this);
    ui->searchLabel->hide();
    ui->searchLineEdit->hide();
    formFiller(acft);
}

//Dialog to be used to create new tail
NewTail::NewTail(QString newreg, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewTail)
{
    ui->setupUi(this);
    ui->editLabel->hide();
    ui->registrationLineEdit->setText(newreg);

    //creating a QMap<id,searchstring>
    auto query = QLatin1String("SELECT make||' '||model||'-'||variant, aircraft_id FROM aircraft");
    auto vector = db::customQuery(query,2);
    QMap<QString, int> map;
    for (int i = 0; i < vector.length()-2 ; i += 2){
        if(vector[i] != QLatin1String("")){
            map.insert(vector[i],vector[i+1].toInt());
        }
    }
    //creating QStringlist for QCompleter
    auto cl = new completionList(completerTarget::aircraft);

    aircraftlist = cl->list;
    idMap = map;

    QCompleter* completer = new QCompleter(aircraftlist,ui->searchLineEdit);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setFilterMode(Qt::MatchContains);
    ui->searchLineEdit->setCompleter(completer);
    ui->searchLineEdit->setStyleSheet("border: 1px solid blue");
    ui->searchLineEdit->setFocus();
}


NewTail::~NewTail()
{
    delete ui;
}


/// Slots

void NewTail::on_searchLineEdit_textChanged(const QString &arg1)
{
    if(aircraftlist.contains(arg1)){//equivalent to editing finished for this purpose. todo: consider connecing qcompleter activated signal with editing finished slot.

        DEB("Template Selected. aircraft_id is: " << idMap.value(arg1));
        //call autofiller for dialog
        formFiller(aircraft(idMap.value(arg1),aircraft::acft));
        ui->searchLineEdit->setStyleSheet("border: 1px solid green");
    }else{
        //for example, editing finished without selecting a result from Qcompleter
        ui->searchLineEdit->setStyleSheet("border: 1px solid orange");
    }
}

void NewTail::on_operationComboBox_currentIndexChanged(int index)
{
    if(index != 0){
        ui->operationComboBox->setStyleSheet("");
    }
}

void NewTail::on_ppTypeComboBox_currentIndexChanged(int index)
{
    if(index != 0){
        ui->ppTypeComboBox->setStyleSheet("");
    }
}

void NewTail::on_ppNumberComboBox_currentIndexChanged(int index)
{
    if(index != 0){
        ui->ppNumberComboBox->setStyleSheet("");
    }
}

void NewTail::on_weightComboBox_currentIndexChanged(int index)
{
    if(index != 0){
        ui->weightComboBox->setStyleSheet("");
    }
}

void NewTail::on_buttonBox_accepted()
{
    DEB("Button Box Accepted.");
    if(ui->registrationLineEdit->text().isEmpty()){
        auto nope = new QMessageBox(this);
        nope->setText("Registration cannot be empty.");
        nope->show();
    }else{
        if(verify()){
            DEB("Form verified");
            auto acOut = createAircraftFromSelection();
            //commit to database
            //accept();
        }else{
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Warning",
                                          "Some recommended fields are empty.\n\n"
                                          "If you do not fill out the aircraft details, "
                                          "it will be impossible to automatically determine Single/Multi Pilot Times or Single/Multi Engine Time."
                                          "This will also impact statistics and auto-logging capabilites.\n\n"
                                          "It is highly recommended to fill in all the details.\n\n"
                                          "Are you sure you want to proceed?",
                                          QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes)
            {
                //auto acOut = createAircraftFromSelection();
                //commit to database (?)
                //accept();
            }
        }
    }
}

/// Functions

/*!
 * \brief NewTail::formFiller populates the Dialog with the
 * information contained in an aircraft object.
 * \param ac
 */
void NewTail::formFiller(aircraft ac)
{
    DEB("Filling Form for a/c" << ac);
    if(!ac.registration.isEmpty()){
        ui->registrationLineEdit->setText(ac.registration);
    }

    ui->companyLineEdit->setText(ac.company);
    ui->makeLineEdit->setText(ac.make);
    ui->modelLineEdit->setText(ac.model);
    ui->variantLineEdit->setText(ac.variant);

    if(ac.singlepilot){
        ui->operationComboBox->setCurrentIndex(1);
    }else if(ac.multipilot){
        ui->operationComboBox->setCurrentIndex(2);
    }

    if(ac.unpowered){
        ui->ppTypeComboBox->setCurrentIndex(1);
    }else if(ac.piston){
        ui->ppTypeComboBox->setCurrentIndex(2);
    }else if(ac.turboprop){
        ui->ppTypeComboBox->setCurrentIndex(3);
    }else if(ac.jet){
        ui->ppTypeComboBox->setCurrentIndex(4);
    }

    if(ac.singleengine){
        ui->ppNumberComboBox->setCurrentIndex(1);
    }else if(ac.multiengine){
        ui->ppNumberComboBox->setCurrentIndex(2);
    }

    if(ac.light){
        ui->weightComboBox->setCurrentIndex(1);
    }else if(ac.medium){
        ui->weightComboBox->setCurrentIndex(2);
    }else if(ac.heavy){
        ui->weightComboBox->setCurrentIndex(3);
    }else if (ac.super) {
        ui->weightComboBox->setCurrentIndex(4);
    }
}

/*!
 * \brief NewTail::verify A simple check for empty recommended fields in the form
 * \return true if all reconmmended fields are populated
 */
bool NewTail::verify()
{
    auto recommended_line_edits = parent()->findChildren<QLineEdit*>("registrationLineEdit");
    recommended_line_edits.append(parent()->findChild<QLineEdit*>("makeLineEdit"));
    recommended_line_edits.append(parent()->findChild<QLineEdit*>("modelLineEdit"));

    auto recommended_combo_boxes = parent()->findChildren<QComboBox*>("operationComboBox");
    recommended_combo_boxes.append(parent()->findChild<QComboBox*>("ppNumberComboBox"));
    recommended_combo_boxes.append(parent()->findChild<QComboBox*>("ppTypeComboBox"));

    for(const auto le : recommended_line_edits){
        if(le->text() != ""){
            recommended_line_edits.removeOne(le);
            le->setStyleSheet("");
        }else{
            le->setStyleSheet("border: 1px solid red");
            DEB("Not Good: " << le);
        }
    }
    for(const auto cb : recommended_combo_boxes){
        if(cb->currentIndex() != 0){

            recommended_combo_boxes.removeOne(cb);
            cb->setStyleSheet("");
        }else{
            cb->setStyleSheet("background: orange");
            DEB("Not Good: " << cb);
        }
    }

    if(recommended_line_edits.isEmpty() && recommended_combo_boxes.isEmpty()){
        return true;
    }else{
        return false;
    }
}

/*!
 * \brief NewTail::createAircraftFromSelection Creates an aircraft object
 * from the current ui selections
 * \return
 */
aircraft NewTail::createAircraftFromSelection()
{
    auto newacft = aircraft();
    newacft.registration = ui->registrationLineEdit->text();
    newacft.company = ui->companyLineEdit->text();
    newacft.make = ui->makeLineEdit->text();
    newacft.model = ui->modelLineEdit->text();
    newacft.variant = ui->variantLineEdit->text();

    if(ui->operationComboBox->currentIndex() == 1){
        newacft.singlepilot = true;
    }else if(ui->operationComboBox->currentIndex() == 2){
        newacft.multipilot = true;
    }

    if(ui->ppNumberComboBox->currentIndex() == 1){
        newacft.singleengine = true;
    }else if(ui->ppNumberComboBox->currentIndex() == 2){
        newacft.multiengine = true;
    }

    if(ui->ppTypeComboBox->currentIndex() == 1){
        newacft.unpowered = true;
    }else if (ui->ppTypeComboBox->currentIndex() == 2) {
        newacft.piston = true;
    }else if (ui->ppTypeComboBox->currentIndex() == 3) {
        newacft.turboprop = true;
    }else if (ui->ppTypeComboBox->currentIndex() == 4) {
        newacft.jet = true;
    }

    if(ui->weightComboBox->currentIndex() == 1){
        newacft.light = true;
    }else if (ui->weightComboBox->currentIndex() == 2) {
        newacft.medium = true;
    }else if (ui->weightComboBox->currentIndex() == 3) {
        newacft.heavy = true;
    }
    return newacft;
}
