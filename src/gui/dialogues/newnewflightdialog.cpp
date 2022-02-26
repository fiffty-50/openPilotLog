#include "newnewflightdialog.h"
#include "ui_newnewflightdialog.h"
#include "src/opl.h"
#include "src/functions/alog.h"
#include "src/functions/adate.h"
#include "src/classes/asettings.h"
#include "src/functions/acalc.h"
#include "src/functions/adatetime.h"
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
    init();
}

NewNewFlightDialog::~NewNewFlightDialog()
{
    delete ui;
}

void NewNewFlightDialog::init()
{
    // Initialise line edit lists
    timeLineEdits = {ui->tofbTimeLineEdit, ui->tonbTimeLineEdit};
    locationLineEdits = {ui->deptLocationLineEdit, ui->destLocationLineEdit};
    pilotNameLineEdits = {ui->picNameLineEdit, ui->sicNameLineEdit, ui->thirdPilotNameLineEdit};

    // {doft = 0, dept = 1, dest = 2, tofb = 3, tonb = 4, pic = 5, acft = 6}
    mandatoryLineEdits = {ui->doftLineEdit, ui->deptLocationLineEdit, ui->destLocationLineEdit,
                          ui->tofbTimeLineEdit, ui->tonbTimeLineEdit,
                          ui->picNameLineEdit, ui->acftLineEdit};

    setupRawInputValidation();
    setupSignalsAndSlots();
    //readSettings();

    ui->doftLineEdit->setText(QDate::currentDate().toString(Qt::ISODate));
    emit ui->doftLineEdit->editingFinished();
}

/*!
 * \brief NewNewFlightDialog::setupRawInputValidation outfits the line edits with QRegularExpresionValidators and QCompleters
 */
void NewNewFlightDialog::setupRawInputValidation()
{
    // Time Line Edits
    for (const auto& line_edit : qAsConst(timeLineEdits)) {
        auto validator = new QRegularExpressionValidator(QRegularExpression("([01]?[0-9]|2[0-3]):?[0-5][0-9]?"), line_edit);
        line_edit->setValidator(validator);
    }
    // Location Line Edits
    for (const auto& line_edit : qAsConst(locationLineEdits)) {
        auto validator = new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]{1,4}"), line_edit);
        line_edit->setValidator(validator);

        auto completer = new QCompleter(completionData.airportList, line_edit);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setCompletionMode(QCompleter::PopupCompletion);
        completer->setFilterMode(Qt::MatchContains);
        line_edit->setCompleter(completer);
    }
    // Name Line Edits
    for (const auto& line_edit : qAsConst(pilotNameLineEdits)) {
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
    for (const auto& line_edit : qAsConst(timeLineEdits))
        QObject::connect(line_edit, &QLineEdit::editingFinished,
                         this, &NewNewFlightDialog::onTimeLineEdit_editingFinished);
    // Change text to upper case for location and acft line edits
    QObject::connect(ui->acftLineEdit, &QLineEdit::textChanged,
                     this, &NewNewFlightDialog::toUpper);
    for (const auto& line_edit : qAsConst(locationLineEdits)) {
        QObject::connect(line_edit, &QLineEdit::textChanged,
                         this, &NewNewFlightDialog::toUpper);
        QObject::connect(line_edit, &QLineEdit::editingFinished,
                         this, &NewNewFlightDialog::onLocationLineEdit_editingFinished);
    }
    for (const auto& line_edit : qAsConst(pilotNameLineEdits))
        QObject::connect(line_edit, &QLineEdit::editingFinished,
                         this, &NewNewFlightDialog::onPilotNameLineEdit_editingFinshed);
}

void NewNewFlightDialog::onGoodInputReceived(QLineEdit *line_edit)
{
    DEB << line_edit->objectName() << " - Good input received - " << line_edit->text();
    line_edit->setStyleSheet(QString());

    if (mandatoryLineEdits.contains(line_edit))
        validationState.validate(mandatoryLineEdits.indexOf(line_edit));

    if (validationState.timesValid()) {
        updateBlockTimeLabel();
        if (validationState.allValid())
            updateNightCheckBoxes();
    }
        validationState.printValidationStatus();
}

void NewNewFlightDialog::onBadInputReceived(QLineEdit *line_edit)
{
    DEB << line_edit->objectName() << " - Bad input received - " << line_edit->text();
    line_edit->setStyleSheet(QStringLiteral("border: 1px solid red"));
    line_edit->setText(QString());

    if (mandatoryLineEdits.contains(line_edit))
        validationState.invalidate(mandatoryLineEdits.indexOf(line_edit));

    validationState.printValidationStatus();
}

void NewNewFlightDialog::updateBlockTimeLabel()
{
    QTime tblk = ATime::blocktime(ui->tofbTimeLineEdit->text(), ui->tonbTimeLineEdit->text());
    ui->tblkDisplayLabel->setText(ATime::toString(tblk));
}


/*!
 * \brief NewNewFlightDialog::updateNightCheckBoxes updates the check boxes for take-off and landing
 * at night. Returns the number of minutes of night time.
 * \return
 */
void NewNewFlightDialog::updateNightCheckBoxes()
{
    if (!validationState.allValid())
        return;

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
    DEB << "toN" << night_values.takeOffNight;
    DEB << "ldgN" << night_values.landingNight;

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
        onGoodInputReceived(line_edit);
        return;
    }

    if(completionData.pilotsIdMap.key(line_edit->text()) != 0) {
        DEB << "Mapped: " << line_edit->text() << completionData.pilotsIdMap.key(line_edit->text());
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
    TODO << "Add new Pilot...";
    //addNewPilot(line_edit);
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
    WARN("Add new tail..");
}

void NewNewFlightDialog::on_doftLineEdit_editingFinished()
{
    const auto line_edit = ui->doftLineEdit;
    auto text = ui->doftLineEdit->text();
    auto label = ui->doftDisplayLabel;
    //DEB << line_edit->objectName() << "Editing finished - " << text;

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

void NewNewFlightDialog::on_buttonBox_accepted()
{
    // emit editing finished for all mandatory line edits to trigger input verification
    for (const auto &line_edit : qAsConst(mandatoryLineEdits)) {
        emit line_edit->editingFinished();
    }

    // If input verification is passed, continue, otherwise prompt user to correct
    // enum ValidationItem {doft = 0, dept = 1, dest = 2, tofb = 3, tonb = 4, pic = 5, acft = 6};
    if (!validationState.allValid()) {
        const auto display_names = QMap<ValidationItem, QString> {
            {ValidationItem::doft, QObject::tr("Date of Flight")},      {ValidationItem::dept, QObject::tr("Departure Airport")},
            {ValidationItem::dest, QObject::tr("Destination Airport")}, {ValidationItem::tofb, QObject::tr("Time Off Blocks")},
            {ValidationItem::tonb, QObject::tr("Time on Blocks")},      {ValidationItem::pic, QObject::tr("PIC Name")},
            {ValidationItem::acft, QObject::tr("Aircraft Registration")}
        };
        QString missing_items;
        for (int i=0; i < mandatoryLineEdits.size(); i++) {
            if (!validationState.validAt(i)){
                missing_items.append(display_names.value(static_cast<ValidationItem>(i)) + "<br>");
                mandatoryLineEdits[i]->setStyleSheet(QStringLiteral("border: 1px solid red"));
            }
        }

        INFO(tr("Not all mandatory entries are valid.<br>"
                "The following item(s) are empty or missing:"
                "<br><br><center><b>%1</b></center><br>"
                "Please go back and fill in the required data."
                ).arg(missing_items));
        return;
    }


    // If input verification passed, collect input and submit to database
    //auto newData = collectInput();
    //DEB << "Setting Data for flightEntry...";
    //flightEntry.setData(newData);
    //DEB << "Committing...";
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
