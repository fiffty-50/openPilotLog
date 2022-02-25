#include "newnewflightdialog.h"
#include "ui_newnewflightdialog.h"
#include "src/opl.h"
#include "src/functions/alog.h"
#include <QDateTime>
#include <QCompleter>

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
    QList<QLineEdit*> time_line_edits = {ui->tofbTimeLineEdit, ui->tonbTimeLineEdit};
    timeLineEdits = &time_line_edits;
    QList<QLineEdit*> location_line_edits = {ui->deptLocationLineEdit, ui->destLocationLineEdit};
    locationLineEdits = &location_line_edits;
    QList<QLineEdit*> pilot_name_line_edits = {ui->picNameLineEdit, ui->sicNameLineEdit, ui->thirdPilotNameLineEdit};
    pilotNameLineEdits = &pilot_name_line_edits;

    setupRawInputValidation();
    //setupSignalsAndSlots();
    //readSettings();

    ui->doftLineEdit->setText(QDate::currentDate().toString(Qt::ISODate));
}

void NewNewFlightDialog::setupRawInputValidation()
{
    for (const auto& line_edit : *timeLineEdits) {
        DEB << "Setting up: " << line_edit->objectName();
        auto validator = new QRegularExpressionValidator(QRegularExpression("([01]?[0-9]|2[0-3]):?[0-5][0-9]?"), line_edit);
        line_edit->setValidator(validator);
    }

    for (const auto& line_edit : *locationLineEdits) {
        auto validator = new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]{1,4}"), line_edit);
        line_edit->setValidator(validator);

        auto completer = new QCompleter(completionData.airportList, line_edit);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setCompletionMode(QCompleter::PopupCompletion);
        completer->setFilterMode(Qt::MatchContains);
        line_edit->setCompleter(completer);
    }

    for (const auto& line_edit : *pilotNameLineEdits) {
        auto completer = new QCompleter(completionData.pilotList, line_edit);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setCompletionMode(QCompleter::PopupCompletion);
        completer->setFilterMode(Qt::MatchContains);
        line_edit->setCompleter(completer);
    }
}
