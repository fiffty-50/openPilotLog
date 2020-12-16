#include "expnewflightdialog.h"
#include "ui_expnewflightdialog.h"

using namespace experimental;

static const auto IATA_RX = QLatin1String("[a-zA-Z0-9]{3}");
static const auto ICAO_RX = QLatin1String("[a-zA-Z0-9]{4}");
//static const auto NAME_RX = QLatin1String("(\\p{L}+('|\\-)?)");//(\\p{L}+(\\s|'|\\-)?\\s?(\\p{L}+)?\\s?)
static const auto NAME_RX = QLatin1String("(\\p{L}+('|\\-|,)?\\p{L}+?)");
static const auto ADD_NAME_RX = QLatin1String("(\\s?(\\p{L}+('|\\-|,)?\\p{L}+?))?");
//static const auto ADD_NAME_RX = QLatin1String("(\\s?(\\p{L}+('|\\-)?))?");
static const auto SELF_RX = QLatin1String("(self|SELF)");
//static const auto NON_WORD_CHAR = QLatin1String("\\W");

/// Raw Input validation
static const auto TIME_VALID_RGX       = QRegularExpression("([01]?[0-9]|2[0-3]):?[0-5][0-9]?");// We only want to allow inputs that make sense as a time, e.g. 99:99 is not a valid time
static const auto LOC_VALID_RGX        = QRegularExpression(IATA_RX + "|" + ICAO_RX);
static const auto AIRCRAFT_VALID_RGX   = QRegularExpression("\\w+\\-?(\\w+)?");
static const auto NAME_VALID_RGX = QRegularExpression(SELF_RX + QLatin1Char('|')
                                                      + NAME_RX + ADD_NAME_RX + ADD_NAME_RX + ADD_NAME_RX + ",?\\s?" // up to 4 first names
                                                      + NAME_RX + ADD_NAME_RX + ADD_NAME_RX + ADD_NAME_RX );// up to 4 last names
//static const auto NAME_VALID_RGX = QRegularExpression("(\\p{L}+('|\\-|,)?\\p{L}+?)");
static const auto DATE_VALID_RGX       = QRegularExpression("^([1-9][0-9]{3}).?(1[0-2]|0[1-9]).?(3[01]|0[1-9]|[12][0-9])?$");// . allows all seperators, still allows for 2020-02-31, additional verification via QDate::isValid()


/// [F] The general idea for this dialog is this:
/// - line edits have validators and completers.
/// - mandatory line edits only emit editing finished if their content is
/// valid. This means mapping user inputs to database keys where required.
/// - A QBitArray is mainained with the state of the mandatory line edits
/// - The deducted entries are automatically filled if all mandatory entries
/// are valid.
///
/// if the user presses "OK", check if all mandatory inputs are valid,
/// check if optional user inputs are valid and commit.
///
/// For the completion and mapping, I have settled on a more low-level approach using
/// Completers based on QStringLists and mapping with QMaps.
///
/// I implemented the Completers and mapping based on a QSqlTableModel which would
/// have been quite nice, since it would keep all data in one place, but as we have
/// seen before with the more high-level qt classes, they are quite slow on execution.
/// Mapping a registration to an ID for example took around 300ms, which is very
/// noticeable in the UI and not an acceptable user experience. Using QStringLists and QMaps
/// this goes down to around 5ms.

ExpNewFlightDialog::ExpNewFlightDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExpNewFlightDialog)
{
    ui->setupUi(this);
    flightEntry = AFlightEntry();
    setupRawInputValidation();
    setupLineEditSignalsAndSlots();
}

ExpNewFlightDialog::ExpNewFlightDialog(int row_id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExpNewFlightDialog)
{
    ui->setupUi(this);
    flightEntry = aDB()->getFlightEntry(row_id);
    setupRawInputValidation();
    setupLineEditSignalsAndSlots();
}

ExpNewFlightDialog::~ExpNewFlightDialog()
{
    delete ui;
}

void ExpNewFlightDialog::onGoodInputReceived(QLineEdit *line_edit)
{
    DEB(line_edit->objectName() << " - Good input received - " << line_edit->text());
    if (mandatoryLineEdits.contains(line_edit)) {
        mandatoryLineEditsGood.setBit(mandatoryLineEdits.indexOf(line_edit), true);
        DEB("Mandatory Good: " << mandatoryLineEditsGood.count(true) << " out of "
            << mandatoryLineEditsGood.size() << ". Array: " << mandatoryLineEditsGood);
    }
}

void ExpNewFlightDialog::onBadInputReceived(QLineEdit *line_edit)
{
    DEB(line_edit->objectName() << " - Bad input received - " << line_edit->text());

    if (mandatoryLineEdits.contains(line_edit)) {
        mandatoryLineEditsGood.setBit(mandatoryLineEdits.indexOf(line_edit), false);
        DEB("Mandatory Good: " << mandatoryLineEditsGood.count(true) << " out of "
            << mandatoryLineEditsGood.size() << ". Array: " << mandatoryLineEditsGood);
    }
}

void ExpNewFlightDialog::setupRawInputValidation()
{
    /////////////////////////////////////// DEBUG /////////////////////////////////////////////////////
    ui->doftLineEdit_2->setText(QDate::currentDate().toString(Qt::ISODate)); // deal with date input later...
    /////////////////////////////////////// DEBUG /////////////////////////////////////////////////////
    // get Maps
    pilotsIdMap      = aDB()->getIdMap(ADataBase::pilots);
    tailsIdMap       = aDB()->getIdMap(ADataBase::tails);
    airportIcaoIdMap = aDB()->getIdMap(ADataBase::airport_identifier_icao);
    airportIataIdMap = aDB()->getIdMap(ADataBase::airport_identifier_iata);
    airportNameIdMap = aDB()->getIdMap(ADataBase::airport_names);
    //get Completer Lists
    pilotList   = aDB()->getCompletionList(ADataBase::pilots);
    tailsList   = aDB()->getCompletionList(ADataBase::registrations);
    airportList = aDB()->getCompletionList(ADataBase::airport_identifier_all);
    auto tempList = QStringList();
    // define tuples
    const std::tuple<QString, QStringList*, QRegularExpression>
            location_line_edit_settings = {"Loc", &airportList, LOC_VALID_RGX};
    const std::tuple<QString, QStringList*, QRegularExpression>
            name_line_edit_settings = {"Name", &pilotList, NAME_VALID_RGX};
    const std::tuple<QString, QStringList*, QRegularExpression>
            acft_line_edit_settings = {"acft", &tailsList, AIRCRAFT_VALID_RGX};
    const std::tuple<QString, QStringList*, QRegularExpression>
            time_line_edit_settings = {"Time", &tempList, TIME_VALID_RGX};
    const QList<std::tuple<QString, QStringList*, QRegularExpression>> line_edit_settings = {
        location_line_edit_settings,
        name_line_edit_settings,
        acft_line_edit_settings,
        time_line_edit_settings
    };
    //get line edits, set up completers and validators
    auto line_edits = ui->flightDataTab_2->findChildren<QLineEdit*>();

    for (const auto &item : line_edit_settings) {
        for (const auto &line_edit : line_edits) {
            if(line_edit->objectName().contains(std::get<0>(item))) {
                DEB("Setting up: " << line_edit->objectName());
                // Set Validator
                auto validator = new QRegularExpressionValidator(std::get<2>(item), line_edit);
                line_edit->setValidator(validator);
                // Set Completer
                auto completer = new QCompleter(*std::get<1>(item), line_edit);
                completer->setCaseSensitivity(Qt::CaseInsensitive);
                completer->setCompletionMode(QCompleter::PopupCompletion);
                completer->setFilterMode(Qt::MatchContains);
                line_edit->setCompleter(completer);
            }
        }
    }
    // populate Mandatory Line Edits list and prepare QBitArray
    mandatoryLineEdits = {
        ui->doftLineEdit_2,
        ui->deptLocLineEdit_2,
        ui->destLocLineEdit_2,
        ui->tofbTimeLineEdit_2,
        ui->tonbTimeLineEdit_2,
        ui->picNameLineEdit_2,
        ui->acftLineEdit_2,
    };
    mandatoryLineEditsGood.resize(mandatoryLineEdits.size());
    /////////////////////////////////////// DEBUG /////////////////////////////////////////////////////
    mandatoryLineEditsGood.setBit(0, true); // deal with date input later...
    /////////////////////////////////////// DEBUG /////////////////////////////////////////////////////
}

void ExpNewFlightDialog::setupLineEditSignalsAndSlots()
{

    auto line_edits = this->findChildren<QLineEdit*>();

    for(const auto &line_edit : line_edits){
        /////////////////////////////////////// DEBUG /////////////////////////////////////////////////////
        QObject::connect(line_edit, &QLineEdit::inputRejected,
                         this, &ExpNewFlightDialog::onInputRejected);
        /////////////////////////////////////// DEBUG /////////////////////////////////////////////////////
        if(line_edit->objectName().contains("Loc") || line_edit->objectName().contains("acft")){
            QObject::connect(line_edit, &QLineEdit::textChanged,
                             this, &ExpNewFlightDialog::onTextChangedToUpper);
        }
        if(line_edit->objectName().contains("acft")){
            QObject::connect(line_edit, &QLineEdit::textChanged,
                             this, &ExpNewFlightDialog::onTextChangedToUpper);
        }
        if(line_edit->objectName().contains("Name")){
            QObject::connect(line_edit, &QLineEdit::editingFinished,
                             this, &ExpNewFlightDialog::onPilotLineEdit_editingFinished);
        }
    }
    QObject::connect(this, &ExpNewFlightDialog::goodInputReceived,
                     this, &ExpNewFlightDialog::onGoodInputReceived);
    QObject::connect(this, &ExpNewFlightDialog::badInputReceived,
                     this, &ExpNewFlightDialog::onBadInputReceived);
    QObject::connect(this, &ExpNewFlightDialog::locationEditingFinished,
                     this, &ExpNewFlightDialog::onLocLineEdit_editingFinished);
    QObject::connect(this, &ExpNewFlightDialog::timeEditingFinished,
                     this, &ExpNewFlightDialog::onTimeLineEdit_editingFinished);
}

// Location Line Edits

void ExpNewFlightDialog::on_deptLocLineEdit_2_editingFinished()
{
    emit locationEditingFinished(ui->deptLocLineEdit_2, ui->deptNameLabel_2);
}

void ExpNewFlightDialog::on_destLocLineEdit_2_editingFinished()
{
    emit locationEditingFinished(ui->destLocLineEdit_2, ui->destNameLabel_2);
}

void ExpNewFlightDialog::onLocLineEdit_editingFinished(QLineEdit *line_edit, QLabel *name_label)
{
    const auto &text = line_edit->text();
    DEB(line_edit->objectName() << " Editing finished. " << text);
    int airport_id = 0;

    // try to map iata or icao code to airport id;
    if (text.length() == 3) {
        airport_id = airportIataIdMap.value(text);
    } else {
        airport_id = airportIcaoIdMap.value(text);
    }
    // check result
    if (airport_id == 0) {
        // to do: prompt user how to handle unknown airport
        name_label->setText("Unknown airport identifier");
        emit badInputReceived(line_edit);
        return;
    }
    line_edit->setText(airportIcaoIdMap.key(airport_id));
    name_label->setText(airportNameIdMap.key(airport_id));
    emit goodInputReceived(line_edit);
}
// Time Line Edits

void ExpNewFlightDialog::on_tofbTimeLineEdit_2_editingFinished()
{
    emit timeEditingFinished(ui->tofbTimeLineEdit_2);
}

void ExpNewFlightDialog::on_tonbTimeLineEdit_2_editingFinished()
{
    emit timeEditingFinished(ui->tonbTimeLineEdit_2);
}

void ExpNewFlightDialog::onTimeLineEdit_editingFinished(QLineEdit *line_edit)
{
    line_edit->setText(ACalc::formatTimeInput(line_edit->text()));
    const auto time = QTime::fromString(line_edit->text(),"hh:mm");
    if(time.isValid()){
        emit goodInputReceived(line_edit);
    } else {
        emit badInputReceived(line_edit);
    }
}


// capitalize input for dept, dest and registration input
void ExpNewFlightDialog::onTextChangedToUpper(const QString &text)
{
    auto sender_object = sender();
    auto line_edit = this->findChild<QLineEdit*>(sender_object->objectName());
    DEB("Text changed - " << line_edit->objectName() << line_edit->text());
    {
        const QSignalBlocker blocker(line_edit);
        line_edit->setText(text.toUpper());
    }
}
// Aircraft Line Edit
void ExpNewFlightDialog::on_acftLineEdit_2_editingFinished()
{
    auto line_edit = ui->acftLineEdit_2;
    DEB(line_edit->objectName() << "Editing Finished!" << line_edit->text());

    if (!line_edit->completer()->currentCompletion().isEmpty()) {
        DEB("Trying to fix input...");
        line_edit->setText(line_edit->completer()->currentCompletion());
    }

    if (tailsIdMap.value(line_edit->text()) == 0) {
        emit badInputReceived(line_edit);
        ui->acftTypeLabel_2->setText("Unknown Registration.");
        // to do: promp user to add new
    } else {
        emit goodInputReceived(line_edit);
    }
}
// Pilot Line Edits
void ExpNewFlightDialog::onPilotLineEdit_editingFinished()
{
    auto sender_object = sender();
    auto line_edit = this->findChild<QLineEdit*>(sender_object->objectName());
    DEB(line_edit->objectName() << "Editing Finished -" << line_edit->text());

    if (!line_edit->completer()->currentCompletion().isEmpty()) {
        DEB("Trying to fix input...");
        line_edit->setText(line_edit->completer()->currentCompletion());
    }
    if(pilotsIdMap.value(line_edit->text()) == 0) {
        emit badInputReceived(line_edit);
        // to do: prompt user to add new
    } else {
        emit goodInputReceived(line_edit);
    }
}




/////////////////////////////////////// DEBUG /////////////////////////////////////////////////////
void ExpNewFlightDialog::onInputRejected()
{
    auto sender_object = sender();
    auto line_edit = this->findChild<QLineEdit*>(sender_object->objectName());
    DEB(line_edit->objectName() << "Input Rejected - " << line_edit->text());
}
/////////////////////////////////////// DEBUG /////////////////////////////////////////////////////


