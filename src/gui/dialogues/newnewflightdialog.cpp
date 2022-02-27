#include "newnewflightdialog.h"
#include "ui_newnewflightdialog.h"
#include "src/opl.h"
#include "src/functions/alog.h"
#include "src/functions/adate.h"
#include "src/classes/asettings.h"
#include "src/functions/acalc.h"
#include "src/functions/adatetime.h"
#include "src/gui/dialogues/newtaildialog.h"
#include "src/gui/dialogues/newpilotdialog.h"
#include <QDateTime>
#include <QCompleter>
#include <QKeyEvent>

NewNewFlightDialog::NewNewFlightDialog(ACompletionData &completion_data,
                                       QWidget *parent)
    : QDialog(parent),
      ui(new Ui::NewNewFlightDialog),
      completionData(completion_data)
{
    ui->setupUi(this);
    flightEntry = AFlightEntry();
    init();
    // Set up UI (New Flight)
    LOG << ASettings::read(ASettings::FlightLogging::Function);
    if(ASettings::read(ASettings::FlightLogging::Function).toString() == QLatin1String("PIC")){
        ui->picNameLineEdit->setText(self);
        ui->functionComboBox->setCurrentIndex(0);
        emit ui->picNameLineEdit->editingFinished();
    }
    if (ASettings::read(ASettings::FlightLogging::Function).toString() == QLatin1String("SIC")) {
        ui->sicNameLineEdit->setText(self);
        ui->functionComboBox->setCurrentIndex(2);
        emit ui->sicNameLineEdit->editingFinished();
    }

    ui->doftLineEdit->setText(QDate::currentDate().toString(Qt::ISODate));
    emit ui->doftLineEdit->editingFinished();
}

NewNewFlightDialog::~NewNewFlightDialog()
{
    delete ui;
}

void NewNewFlightDialog::init()
{
    // Initialise line edit lists
    auto time_line_edits = new QList<QLineEdit*>{ui->tofbTimeLineEdit, ui->tonbTimeLineEdit};
    timeLineEdits = time_line_edits;
    auto location_line_edits = new QList<QLineEdit*>{ui->deptLocationLineEdit, ui->destLocationLineEdit};
    locationLineEdits = location_line_edits;
    auto pilot_name_line_edits = new QList<QLineEdit*>{ui->picNameLineEdit, ui->sicNameLineEdit, ui->thirdPilotNameLineEdit};
    pilotNameLineEdits = pilot_name_line_edits;
    auto mandatory_line_edits = new QList<QLineEdit*>{ui->doftLineEdit, ui->deptLocationLineEdit, ui->destLocationLineEdit,
            ui->tofbTimeLineEdit, ui->tonbTimeLineEdit,
            ui->picNameLineEdit, ui->acftLineEdit};
    // {doft = 0, dept = 1, dest = 2, tofb = 3, tonb = 4, pic = 5, acft = 6}
    mandatoryLineEdits = mandatory_line_edits;

    for (const auto & approach : Opl::ApproachTypes){
        ui->approachComboBox->addItem(approach);
    }

    setupRawInputValidation();
    setupSignalsAndSlots();
    readSettings();
}

/*!
 * \brief NewNewFlightDialog::setupRawInputValidation outfits the line edits with QRegularExpresionValidators and QCompleters
 */
void NewNewFlightDialog::setupRawInputValidation()
{
    // Time Line Edits
    for (const auto& line_edit : *timeLineEdits) {
        auto validator = new QRegularExpressionValidator(QRegularExpression("([01]?[0-9]|2[0-3]):?[0-5][0-9]?"), line_edit);
        line_edit->setValidator(validator);
    }
    // Location Line Edits
    for (const auto& line_edit : *locationLineEdits) {
        auto validator = new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]{1,4}"), line_edit);
        line_edit->setValidator(validator);

        auto completer = new QCompleter(completionData.airportList, line_edit);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setCompletionMode(QCompleter::PopupCompletion);
        completer->setFilterMode(Qt::MatchContains);
        line_edit->setCompleter(completer);
    }
    // Name Line Edits
    for (const auto& line_edit : *pilotNameLineEdits) {
        auto completer = new QCompleter(completionData.pilotList, line_edit);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setCompletionMode(QCompleter::PopupCompletion);
        completer->setFilterMode(Qt::MatchContains);
        line_edit->setCompleter(completer);
    }
    // Acft Line Edit
    auto completer = new QCompleter(completionData.tailsList, ui->acftLineEdit);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setFilterMode(Qt::MatchContains);
    ui->acftLineEdit->setCompleter(completer);

}

void NewNewFlightDialog::setupSignalsAndSlots()
{
    for (const auto& line_edit : *timeLineEdits)
        QObject::connect(line_edit, &QLineEdit::editingFinished,
                         this, &NewNewFlightDialog::onTimeLineEdit_editingFinished);
    // Change text to upper case for location and acft line edits
    QObject::connect(ui->acftLineEdit, &QLineEdit::textChanged,
                     this, &NewNewFlightDialog::toUpper);
    for (const auto& line_edit : *locationLineEdits) {
        QObject::connect(line_edit, &QLineEdit::textChanged,
                         this, &NewNewFlightDialog::toUpper);
        QObject::connect(line_edit, &QLineEdit::editingFinished,
                         this, &NewNewFlightDialog::onLocationLineEdit_editingFinished);
    }
    for (const auto& line_edit : *pilotNameLineEdits)
        QObject::connect(line_edit, &QLineEdit::editingFinished,
                         this, &NewNewFlightDialog::onPilotNameLineEdit_editingFinshed);
}

void NewNewFlightDialog::readSettings()
{
    ASettings settings;
    ui->functionComboBox->setCurrentText(ASettings::read(ASettings::FlightLogging::Function).toString());
    ui->approachComboBox->setCurrentText(ASettings::read(ASettings::FlightLogging::Approach).toString());
    ui->pilotFlyingCheckBox->setChecked(ASettings::read(ASettings::FlightLogging::PilotFlying).toBool());
    ui->ifrCheckBox->setChecked(ASettings::read(ASettings::FlightLogging::LogIFR).toBool());
    ui->flightNumberLineEdit->setText(ASettings::read(ASettings::FlightLogging::FlightNumberPrefix).toString());

}

void NewNewFlightDialog::onGoodInputReceived(QLineEdit *line_edit)
{
    DEB << line_edit->objectName() << " - Good input received - " << line_edit->text();
    line_edit->setStyleSheet(QString());

    if (mandatoryLineEdits->contains(line_edit))
        validationState.validate(mandatoryLineEdits->indexOf(line_edit));

    if (validationState.timesValid()) {
        updateBlockTimeLabel();
        if (validationState.nightDataValid())
            updateNightCheckBoxes();
    }
        validationState.printValidationStatus();
}

void NewNewFlightDialog::onBadInputReceived(QLineEdit *line_edit)
{
    DEB << line_edit->objectName() << " - Bad input received - " << line_edit->text();
    line_edit->setStyleSheet(QStringLiteral("border: 1px solid red"));
    line_edit->setText(QString());

    if (mandatoryLineEdits->contains(line_edit))
        validationState.invalidate(mandatoryLineEdits->indexOf(line_edit));

    validationState.printValidationStatus();
}

void NewNewFlightDialog::updateBlockTimeLabel()
{
    QTime tblk = ATime::blocktime(ui->tofbTimeLineEdit->text(), ui->tonbTimeLineEdit->text());
    ui->tblkDisplayLabel->setText(ATime::toString(tblk));
}

/*!
 * \brief NewNewFlightDialog::addNewTail If the user input is not in the aircraftList, the user
 * is prompted if he wants to add a new entry to the database
 */
void NewNewFlightDialog::addNewTail(QLineEdit& parent_line_edit)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("No Aircraft found"),
                                  tr("No aircraft with this registration found.<br>"
                                     "If this is the first time you log a flight with this aircraft, "
                                     "you have to add the registration to the database first."
                                     "<br><br>Would you like to add a new aircraft to the database?"),
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // create and open new aircraft dialog
        NewTailDialog nt(ui->acftLineEdit->text(), this);
        int ret = nt.exec();
        // update map and list, set line edit
        if (ret == QDialog::Accepted) {
            DEB << "New Tail Entry added:";
            DEB << aDB->getTailEntry(aDB->getLastEntry(ADatabaseTable::tails));

            // update completion Data and Completer
            completionData.updateTails();
            auto new_model = new QStringListModel(completionData.tailsList, parent_line_edit.completer());
            parent_line_edit.completer()->setModel(new_model); //setModel deletes old model if it has the completer as parent

            // update Line Edit
            parent_line_edit.setText(completionData.tailsIdMap.value(aDB->getLastEntry(ADatabaseTable::tails)));
            emit parent_line_edit.editingFinished();
        } else {
            parent_line_edit.setText(QString());
            parent_line_edit.setFocus();
        }
    } else {
        parent_line_edit.setText(QString());
        parent_line_edit.setFocus();
    }
}

/*!
 * \brief NewNewFlightDialog::addNewPilot If the user input is not in the pilotNameList, the user
 * is prompted if he wants to add a new entry to the database
 */
void NewNewFlightDialog::addNewPilot(QLineEdit& parent_line_edit)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("No Pilot found"),
                                  tr("No pilot found.<br>Please enter the Name as"
                                     "<br><br><center><b>Lastname, Firstname</b></center><br><br>"
                                     "If this is the first time you log a flight with this pilot, "
                                     "you have to add the pilot to the database first."
                                     "<br><br>Would you like to add a new pilot to the database?"),
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // create and open new pilot dialog
        NewPilotDialog np(this);
        int ret = np.exec();
        // update map and list, set line edit
        if (ret == QDialog::Accepted) {
            DEB << "New Pilot Entry added:";
            DEB << aDB->getPilotEntry(aDB->getLastEntry(ADatabaseTable::pilots));
            // update completion Data and Completer
            completionData.updatePilots();
            auto new_model = new QStringListModel(completionData.pilotList, parent_line_edit.completer());
            parent_line_edit.completer()->setModel(new_model); //setModel deletes old model if it has the completer as parent

            // update Line Edit
            parent_line_edit.setText(completionData.pilotsIdMap.value(aDB->getLastEntry(ADatabaseTable::pilots)));
            emit parent_line_edit.editingFinished();
        } else {
            parent_line_edit.setText(QString());
            parent_line_edit.setFocus();
        }
    } else {
        parent_line_edit.setText(QString());
    }
}

RowData_T NewNewFlightDialog::prepareFlightEntryData()
{
    if(!validationState.allValid())
        return {};

    RowData_T new_data;
    // Calculate Block and Night Time
    const int block_minutes = ATime::blockMinutes(ui->tofbTimeLineEdit->text(), ui->tonbTimeLineEdit->text());
    QDateTime departure_date_time = ADateTime::fromString(ui->doftLineEdit->text() + ui->tofbTimeLineEdit->text());
    const auto night_time_data = ACalc::NightTimeValues(ui->deptLocationLineEdit->text(), ui->destLocationLineEdit->text(),
                           departure_date_time, block_minutes, ASettings::read(ASettings::FlightLogging::NightAngle).toInt());
    // Mandatory data
    new_data.insert(Opl::Db::FLIGHTS_DOFT, ui->doftLineEdit->text());
    new_data.insert(Opl::Db::FLIGHTS_DEPT, ui->deptLocationLineEdit->text());
    new_data.insert(Opl::Db::FLIGHTS_TOFB, ATime::toMinutes(ui->tofbTimeLineEdit->text()));
    new_data.insert(Opl::Db::FLIGHTS_DEST, ui->destLocationLineEdit->text());
    new_data.insert(Opl::Db::FLIGHTS_TONB, ATime::toMinutes(ui->tonbTimeLineEdit->text()));
    new_data.insert(Opl::Db::FLIGHTS_TBLK, block_minutes);
    // Night
    new_data.insert(Opl::Db::FLIGHTS_TNIGHT, night_time_data.nightMinutes);
    // Aircraft
    int acft_id = completionData.tailsIdMap.key(ui->acftLineEdit->text());
    new_data.insert(Opl::Db::FLIGHTS_ACFT, acft_id);
    const ATailEntry acft_data = aDB->getTailEntry(acft_id);
    bool multi_pilot = acft_data.getData().value(Opl::Db::TAILS_MULTIPILOT).toBool();
    bool multi_engine = acft_data.getData().value(Opl::Db::TAILS_MULTIENGINE).toBool();

    if (multi_pilot) {
        new_data.insert(Opl::Db::FLIGHTS_TMP, block_minutes);
        new_data.insert(Opl::Db::FLIGHTS_TSPSE, QString());
        new_data.insert(Opl::Db::FLIGHTS_TSPME, QString());
    } else if (!multi_pilot && !multi_engine) {
        new_data.insert(Opl::Db::FLIGHTS_TMP, QString());
        new_data.insert(Opl::Db::FLIGHTS_TSPSE, block_minutes);
        new_data.insert(Opl::Db::FLIGHTS_TSPME, QString());
    } else if (!multi_pilot && multi_engine) {
        new_data.insert(Opl::Db::FLIGHTS_TMP, QString());
        new_data.insert(Opl::Db::FLIGHTS_TSPSE, QString());
        new_data.insert(Opl::Db::FLIGHTS_TSPME, block_minutes);
    }
    // Pilots
    new_data.insert(Opl::Db::FLIGHTS_PIC, completionData.pilotsIdMap.key(ui->picNameLineEdit->text()));
    new_data.insert(Opl::Db::FLIGHTS_SECONDPILOT, completionData.pilotsIdMap.key(ui->sicNameLineEdit->text()));
    new_data.insert(Opl::Db::FLIGHTS_THIRDPILOT, completionData.pilotsIdMap.key(ui->thirdPilotNameLineEdit->text()));
    // IFR time
    if (ui->ifrCheckBox->isChecked()) {
        new_data.insert(Opl::Db::FLIGHTS_TIFR, block_minutes);
    } else {
        new_data.insert(Opl::Db::FLIGHTS_TIFR, QString());
    }
    // Function Times
    QList<QLatin1String> function_times = {
        Opl::Db::FLIGHTS_TPIC,
        Opl::Db::FLIGHTS_TPICUS,
        Opl::Db::FLIGHTS_TSIC,
        Opl::Db::FLIGHTS_TDUAL,
        Opl::Db::FLIGHTS_TFI,
    };
    for (int i = 0; i < 5; i++) {
        if (i == ui->functionComboBox->currentIndex())
            new_data.insert(function_times[i], block_minutes);
        else
            new_data.insert(function_times[i], QString());
    }
    // Pilot flying / Pilot monitoring
    new_data.insert(Opl::Db::FLIGHTS_PILOTFLYING, ui->pilotFlyingCheckBox->isChecked());
    // Take-Off and Landing
    if (ui->toNightCheckBox->isChecked()) {
        new_data.insert(Opl::Db::FLIGHTS_TONIGHT, ui->takeOffSpinBox->value());
        new_data.insert(Opl::Db::FLIGHTS_TODAY, 0);
    } else {
        new_data.insert(Opl::Db::FLIGHTS_TONIGHT, 0);
        new_data.insert(Opl::Db::FLIGHTS_TODAY, ui->takeOffSpinBox->value());
    }
    if (ui->ldgNightCheckBox->isChecked()) {
        new_data.insert(Opl::Db::FLIGHTS_LDGNIGHT, ui->landingSpinBox->value());
        new_data.insert(Opl::Db::FLIGHTS_LDGDAY, 0);
    } else {
        new_data.insert(Opl::Db::FLIGHTS_LDGNIGHT, 0);
        new_data.insert(Opl::Db::FLIGHTS_LDGDAY, ui->landingSpinBox->value());
    }
    if (ui->approachComboBox->currentText() == Opl::ApproachTypes[3]) // ILS CAT III
        new_data.insert(Opl::Db::FLIGHTS_AUTOLAND, ui->landingSpinBox->value());

    // Additional Data
    new_data.insert(Opl::Db::FLIGHTS_APPROACHTYPE, ui->approachComboBox->currentText());
    new_data.insert(Opl::Db::FLIGHTS_FLIGHTNUMBER, ui->flightNumberLineEdit->text());
    new_data.insert(Opl::Db::FLIGHTS_REMARKS, ui->remarksLineEdit->text());
    return new_data;
}

/*!
 * \brief NewNewFlightDialog::updateNightCheckBoxes updates the check boxes for take-off and landing
 * at night. Returns the number of minutes of night time.
 * \return
 */
void NewNewFlightDialog::updateNightCheckBoxes()
{
    // Calculate Night Time
    const QString dept_date = (ui->doftLineEdit->text() + ui->tofbTimeLineEdit->text());
    const auto dept_date_time = ADateTime::fromString(dept_date);
    const int block_minutes = ATime::blockMinutes(ui->tofbTimeLineEdit->text(), ui->tonbTimeLineEdit->text());
    const int night_angle = ASettings::read(ASettings::FlightLogging::NightAngle).toInt();
    const auto night_values = ACalc::NightTimeValues(
                ui->deptLocationLineEdit->text(),
                ui->destLocationLineEdit->text(),
                dept_date_time,
                block_minutes,
                night_angle);
    // set check boxes
    if (night_values.takeOffNight)
        ui->toNightCheckBox->setCheckState(Qt::Checked);
    else
        ui->toNightCheckBox->setCheckState(Qt::Unchecked);

    if (night_values.landingNight)
        ui->ldgNightCheckBox->setCheckState(Qt::Checked);
    else
        ui->ldgNightCheckBox->setCheckState(Qt::Unchecked);
}

// # Slots
void NewNewFlightDialog::toUpper(const QString &text)
{
    const auto line_edit = this->findChild<QLineEdit*>(sender()->objectName());
    {
        const QSignalBlocker blocker(line_edit);
        line_edit->setText(text.toUpper());
    }
}

void NewNewFlightDialog::onTimeLineEdit_editingFinished()
{
    auto line_edit = this->findChild<QLineEdit*>(sender()->objectName());
    DEB << line_edit->objectName() << "Editing finished -" << line_edit->text();

    const QString time_string = ATime::formatTimeInput(line_edit->text());
    const QTime time = ATime::fromString(time_string);

    if (time.isValid()) {
        line_edit->setText(time_string);
        onGoodInputReceived(line_edit);
    } else {
        onBadInputReceived(line_edit);
        line_edit->setText(QString());
    }

}

void NewNewFlightDialog::onPilotNameLineEdit_editingFinshed()
{
    auto line_edit = this->findChild<QLineEdit*>(sender()->objectName());
    DEB << line_edit->objectName() << "Editing Finished -" << line_edit->text();

    if(line_edit->text().contains(self, Qt::CaseInsensitive)) {
        DEB << "self recognized.";
        line_edit->setText(completionData.pilotsIdMap.value(1));
        if (line_edit->objectName() == QLatin1String("picNameLineEdit"))
            ui->functionComboBox->setCurrentIndex(0);

        onGoodInputReceived(line_edit);
        return;
    }

    int pilot_id = completionData.pilotsIdMap.key(line_edit->text());
    if(pilot_id != 0) {
        DEB << "Mapped: " << line_edit->text() << pilot_id;
        if (line_edit->objectName() == QLatin1String("picNameLineEdit") && pilot_id == 1)
            ui->functionComboBox->setCurrentIndex(0);
        onGoodInputReceived(line_edit);
        return;
    }

    if (line_edit->text().isEmpty()) {
        return;
    }

    if (!line_edit->completer()->currentCompletion().isEmpty()) {
        DEB << "Trying to fix input...";
        line_edit->setText(line_edit->completer()->currentCompletion());
        emit line_edit->editingFinished();
        return;
    }

    onBadInputReceived(line_edit);
    addNewPilot(*line_edit);
}

void NewNewFlightDialog::onLocationLineEdit_editingFinished()
{
    const QString line_edit_name = sender()->objectName();
    const auto line_edit = this->findChild<QLineEdit*>(line_edit_name);
    DEB << line_edit->objectName() << "Editing Finished -" << line_edit->text();
    QLabel* name_label;
    if (line_edit_name.contains(QLatin1String("dept"))) {
        name_label = ui->deptNameLabel;
    } else {
        name_label = ui->destNameLabel;
    }

    const auto &text = line_edit->text();
    int airport_id = 0;

    // try to map iata or icao code to airport id;
    if (text.length() == 3) {
        airport_id = completionData.airportIataIdMap.key(text);
    } else {
        airport_id = completionData.airportIcaoIdMap.key(text);
    }
    // check result
    if (airport_id == 0) {
        // to do: prompt user how to handle unknown airport
        name_label->setText(tr("Unknown airport identifier"));
        onBadInputReceived(line_edit);
        return;
    }
    line_edit->setText(completionData.airportIcaoIdMap.value(airport_id));
    name_label->setText(completionData.airportNameIdMap.value(airport_id));
    onGoodInputReceived(line_edit);
}

void NewNewFlightDialog::on_acftLineEdit_editingFinished()
{
    const auto line_edit = ui->acftLineEdit;
    int acft_id = completionData.tailsIdMap.key(line_edit->text());
    DEB << "acft_id: " << acft_id;

    if (acft_id != 0) { // Success
        auto acft = aDB->getTailEntry(acft_id);
        ui->acftDisplayLabel->setText(acft.type());
        onGoodInputReceived(line_edit);
        return;
    }

    // try to use a completion
    if (!line_edit->completer()->currentCompletion().isEmpty() && !line_edit->text().isEmpty()) {
        line_edit->setText(line_edit->completer()->currentCompletion().split(QLatin1Char(' ')).first());
        emit line_edit->editingFinished();
        return;
    }

    // Mark as bad input and prompt for adding new tail
    onBadInputReceived(line_edit);
    ui->acftDisplayLabel->setText(tr("Unknown Registration."));
    if (!(line_edit->text() == QString()))
        addNewTail(*line_edit);
}

void NewNewFlightDialog::on_doftLineEdit_editingFinished()
{
    const auto line_edit = ui->doftLineEdit;
    auto text = ui->doftLineEdit->text();
    auto label = ui->doftDisplayLabel;

    TODO << "Non-default Date formats not implemented yet.";
    Opl::Date::ADateFormat date_format = Opl::Date::ADateFormat::ISODate;
    auto date = ADate::parseInput(text, date_format);
    if (date.isValid()) {
        label->setText(date.toString(Qt::TextDate));
        line_edit->setText(ADate::toString(date, date_format));
        onGoodInputReceived(line_edit);
        return;
    }

    label->setText(tr("Invalid Date."));
    onBadInputReceived(line_edit);
}

void NewNewFlightDialog::on_pilotFlyingCheckBox_stateChanged(int arg1)
{
    if (arg1 == Qt::CheckState::Checked) {
        ui->takeOffSpinBox->setValue(1);
        ui->landingSpinBox->setValue(1);
    } else {
        ui->takeOffSpinBox->setValue(0);
        ui->landingSpinBox->setValue(0);
    }
}

void NewNewFlightDialog::on_approachComboBox_currentTextChanged(const QString &arg1)
{
    if (arg1 == QLatin1String("OTHER"))
        INFO(tr("Please specify the approach type in the remarks field."));
}

/*!
 * \brief NewNewFlightDialog::on_functionComboBox_currentIndexChanged
 * If the Function Combo Box is selected to be either PIC or SIC, fill the corresponding
 * nameLineEdit with the logbook owner's name, then check for duplication.
 */
void NewNewFlightDialog::on_functionComboBox_currentIndexChanged(int index)
{
    if (index == 0) {
        ui->picNameLineEdit->setText(self);
        emit ui->picNameLineEdit->editingFinished();
        if (completionData.pilotsIdMap.key(ui->picNameLineEdit->text())
         == completionData.pilotsIdMap.key(ui->sicNameLineEdit->text()))
                ui->sicNameLineEdit->setText(QString());
    } else if (index == 2) {
        ui->sicNameLineEdit->setText(self);
        emit ui->sicNameLineEdit->editingFinished();
        if (completionData.pilotsIdMap.key(ui->picNameLineEdit->text())
         == completionData.pilotsIdMap.key(ui->sicNameLineEdit->text()))
            ui->picNameLineEdit->setText(QString());
    }
}

void NewNewFlightDialog::on_buttonBox_accepted()
{
    // If input verification is passed, continue, otherwise prompt user to correct
    if (!validationState.allValid()) {
        const auto display_names = QMap<ValidationItem, QString> {
            {ValidationItem::doft, QObject::tr("Date of Flight")},      {ValidationItem::dept, QObject::tr("Departure Airport")},
            {ValidationItem::dest, QObject::tr("Destination Airport")}, {ValidationItem::tofb, QObject::tr("Time Off Blocks")},
            {ValidationItem::tonb, QObject::tr("Time on Blocks")},      {ValidationItem::pic, QObject::tr("PIC Name")},
            {ValidationItem::acft, QObject::tr("Aircraft Registration")}
        };
        QString missing_items;
        for (int i=0; i < mandatoryLineEdits->size(); i++) {
            if (!validationState.validAt(i)){
                missing_items.append(display_names.value(static_cast<ValidationItem>(i)) + "<br>");
                mandatoryLineEdits->at(i)->setStyleSheet(QStringLiteral("border: 1px solid red"));
            }
        }

        INFO(tr("Not all mandatory entries are valid.<br>"
                "The following item(s) are empty or invalid:"
                "<br><br><center><b>%1</b></center><br>"
                "Please go back and fill in the required data."
                ).arg(missing_items));
        return;
    }


    // If input verification passed, collect input and submit to database
    auto newData = prepareFlightEntryData();

    //DEB << "Setting Data for flightEntry...";
    flightEntry.setData(newData);
    DEB << "Committing: ";
    DEB << flightEntry;
    //if (!aDB->commit(flightEntry)) {
    //    WARN(tr("The following error has ocurred:"
    //                           "<br><br>%1<br><br>"
    //                           "The entry has not been saved."
    //                           ).arg(aDB->lastError.text()));
    //    return;
    //} else {
    //    QDialog::accept();
    //}
}

