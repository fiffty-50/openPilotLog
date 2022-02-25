#include "newnewflightdialog.h"
#include "ui_newnewflightdialog.h"
#include "src/opl.h"
#include "src/functions/alog.h"
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
}

/*!
 * \brief NewNewFlightDialog::eventFilter the event filter is used to invalidate mandatory line edits on entering. This acts as a double-check against
 * false inputs for cases when the editingFinished() signal is not emitted due to the QValidators blocking it.
 */
bool NewNewFlightDialog::eventFilter(QObject* object, QEvent* event)
{
    auto line_edit = qobject_cast<QLineEdit*>(object);
    if (line_edit != nullptr) {
        if (mandatoryLineEdits.contains(line_edit) && event->type() == QEvent::FocusIn) {
            //DEB << "Invalidating " << line_edit->objectName() << "(Focus In Event)";
            validationState.invalidate(mandatoryLineEdits.indexOf(line_edit));
            return false;
        }
    }
    return false;
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

    // install event filter for focus in vent
    for (const auto& line_edit : qAsConst(mandatoryLineEdits)) {
        line_edit->installEventFilter(this);
    }
}

void NewNewFlightDialog::onGoodInputReceived(QLineEdit *line_edit)
{
    DEB << line_edit->objectName() << " - Good input received - " << line_edit->text();
    line_edit->setStyleSheet(QString());

    if (mandatoryLineEdits.contains(line_edit))
        validationState.validate(mandatoryLineEdits.indexOf(line_edit));
    if (validationState.allValid())
        DEB << "All mandatory Line Edits valid!";
        // Go to onMandatoryLineEditsFilled?
    else
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
    TODO << "Fix looking up and matching...";
}
