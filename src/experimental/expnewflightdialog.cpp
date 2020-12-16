#include "expnewflightdialog.h"
#include "ui_expnewflightdialog.h"

using namespace experimental;

static const auto IATA_RX = QLatin1String("[a-zA-Z0-9]{3}");
static const auto ICAO_RX = QLatin1String("[a-zA-Z0-9]{4}");
static const auto NAME_RX = QLatin1String("(\\p{L}+('|\\-)?)");//(\\p{L}+(\\s|'|\\-)?\\s?(\\p{L}+)?\\s?)
static const auto ADD_NAME_RX = QLatin1String("(\\s?(\\p{L}+('|\\-)?))?");
static const auto SELF_RX = QLatin1String("(self|SELF)");

/// Raw Input validation
static const auto TIME_VALID_RGX       = QRegularExpression("([01]?[0-9]|2[0-3]):?[0-5][0-9]?");// We only want to allow inputs that make sense as a time, e.g. 99:99 is not a valid time
static const auto LOC_VALID_RGX        = QRegularExpression(IATA_RX + "|" + ICAO_RX);
static const auto AIRCRAFT_VALID_RGX   = QRegularExpression("[A-Z0-9]+\\-?[A-Z0-9]+");
static const auto NAME_VALID_RGX = QRegularExpression(SELF_RX + QLatin1Char('|')
                                                     + NAME_RX + ADD_NAME_RX + ADD_NAME_RX + ADD_NAME_RX + ",?\\s?" // up to 4 first names
                                                     + NAME_RX + ADD_NAME_RX + ADD_NAME_RX + ADD_NAME_RX );// up to 4 last names
static const auto DATE_VALID_RGX       = QRegularExpression("^([1-9][0-9]{3}).?(1[0-2]|0[1-9]).?(3[01]|0[1-9]|[12][0-9])?$");// . allows all seperators, still allows for 2020-02-31, additional verification via QDate::isValid()


ExpNewFlightDialog::ExpNewFlightDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExpNewFlightDialog)
{
    ui->setupUi(this);
    flightEntry = AFlightEntry();
    setupRawInputValidation();
    setupMandatoryLineEdits();
}

ExpNewFlightDialog::ExpNewFlightDialog(int row_id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExpNewFlightDialog)
{
    ui->setupUi(this);
    flightEntry = aDB()->getFlightEntry(row_id);
    setupRawInputValidation();
    setupMandatoryLineEdits();
}

ExpNewFlightDialog::~ExpNewFlightDialog()
{
    delete ui;
}

void ExpNewFlightDialog::onGoodInputReceived(QLineEdit *line_edit)
{
    DEB(line_edit->objectName() << " - Good input received - " << line_edit->text());
    mandatoryLineEditsGood.setBit(mandatoryLineEdits.indexOf(line_edit), true);
    DEB("Mandatory Good: " << mandatoryLineEditsGood);
}

void ExpNewFlightDialog::onBadInputReceived(QLineEdit *line_edit)
{
    DEB(line_edit->objectName() << " - Bad input received - " << line_edit->text());
    mandatoryLineEditsGood.setBit(mandatoryLineEdits.indexOf(line_edit), false);
    DEB("Mandatory Good: " << mandatoryLineEditsGood);
}

void ExpNewFlightDialog::setupRawInputValidation()
{
    //get Completer Lists
    pilotList   = aDB()->getCompletionList(ADataBase::pilots);
    tailsList   = aDB()->getCompletionList(ADataBase::registrations);
    airportList = aDB()->getCompletionList(ADataBase::airport_identifier);
    // define tuples
    const std::tuple<QString, QStringList*, QRegularExpression>
            location_line_edit_settings = {"Loc", &airportList, LOC_VALID_RGX};
    const std::tuple<QString, QStringList*, QRegularExpression>
            name_line_edit_settings = {"Name", &pilotList, NAME_VALID_RGX};
    const std::tuple<QString, QStringList*, QRegularExpression>
            acft_line_edit_settings = {"acft", &tailsList, AIRCRAFT_VALID_RGX};
    const QList<std::tuple<QString, QStringList*, QRegularExpression>> line_edit_settings = {
        location_line_edit_settings,
        name_line_edit_settings,
        acft_line_edit_settings
    };

    //get line edits, set up completers and validators
    auto line_edits = ui->flightDataTab_2->findChildren<QLineEdit*>();

    for (const auto &item : line_edit_settings) {
        for (const auto &line_edit : line_edits) {
            if(line_edit->objectName().contains(std::get<0>(item))) {
                DEB("Setting up: " << line_edit->objectName());
                auto completer = new QCompleter(*std::get<1>(item), line_edit);
                completer->setCaseSensitivity(Qt::CaseInsensitive);
                completer->setCompletionMode(QCompleter::PopupCompletion);
                completer->setFilterMode(Qt::MatchContains);
                auto validator = new AStrictRxValidator(std::get<2>(item), line_edit);
                line_edit->setCompleter(completer);
                line_edit->setValidator(validator);
            }
        }
    }
    // get Maps
    pilotsIdMap = aDB()->getIdMap(ADataBase::pilots);
    tailsIdMap  = aDB()->getIdMap(ADataBase::tails);
    airportIdentifierIdMap = aDB()->getIdMap(ADataBase::airport_identifier);
    airportNameIdMap = aDB()->getIdMap(ADataBase::airport_names);
}

void ExpNewFlightDialog::setupMandatoryLineEdits()
{
    QObject::connect(this, &ExpNewFlightDialog::goodInputReceived,
                     this, &ExpNewFlightDialog::onGoodInputReceived);
    QObject::connect(this, &ExpNewFlightDialog::badInputReceived,
                     this, &ExpNewFlightDialog::onBadInputReceived);
    this->mandatoryLineEdits = {
        ui->doftLineEdit_2,
        ui->deptLocLineEdit_2,
        ui->destLocLineEdit_2,
        ui->tofbTimeLineEdit_2,
        ui->tonbTimeLineEdit_2,
        ui->picNameLineEdit_2,
        ui->acftLineEdit_2,
    };
    mandatoryLineEditsGood.resize(mandatoryLineEdits.size());
    DEB("Mandatory Good: " << mandatoryLineEditsGood);

    for(const auto &line_edit : mandatoryLineEdits){
        QObject::connect(line_edit, &QLineEdit::inputRejected,
                         this, &ExpNewFlightDialog::onInputRejected);
        if(line_edit->objectName().contains("Loc") || line_edit->objectName().contains("acft")){
            QObject::connect(line_edit, &QLineEdit::textChanged,
                             this, &ExpNewFlightDialog::onTextChangedToUpper);
        }
    }
}

void ExpNewFlightDialog::on_deptLocLineEdit_2_editingFinished()
{
    auto line_edit = ui->deptLocLineEdit_2;
    const auto &text = line_edit->text();

    if (airportIdentifierIdMap.value(text) != 0) {
        ui->deptNameLabel_2->setText(airportNameIdMap.key(airportIdentifierIdMap.value(text)));
        emit goodInputReceived(line_edit);
    } else {
        emit badInputReceived(line_edit);
    }
}

void ExpNewFlightDialog::on_destLocLineEdit_2_editingFinished()
{
    auto line_edit = ui->destLocLineEdit_2;
    const auto &text = line_edit->text();
    if (airportIdentifierIdMap.value(text) != 0) {
        ui->destNameLabel_2->setText(airportNameIdMap.key(airportIdentifierIdMap.value(text)));
        emit goodInputReceived(line_edit);
    } else {
        emit badInputReceived(line_edit);
    }
}

void ExpNewFlightDialog::onInputRejected()
{
    auto sender_object = sender();
    auto line_edit = this->findChild<QLineEdit*>(sender_object->objectName());
    DEB(line_edit->objectName() << "Input Rejected - " << line_edit->text());
    {
        const QSignalBlocker blocker(line_edit);
        auto text = line_edit->text();
        line_edit->setText(text);
        if(line_edit->objectName().contains("Loc") || line_edit->objectName().contains("acft"))
            line_edit->setText(text.toUpper());
    }
    emit badInputReceived(line_edit);
}

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

void ExpNewFlightDialog::on_acftLineEdit_2_editingFinished()
{
    DEB(sender()->objectName() << "Editing Finished!");
    auto line_edit = ui->acftLineEdit_2;
    line_edit->setText(line_edit->completer()->currentCompletion());

    if(tailsIdMap.value(line_edit->text()) == 0) {
        emit badInputReceived(line_edit);
        // to do: promp user to add new
    } else {
        emit goodInputReceived(line_edit);
    }
}
