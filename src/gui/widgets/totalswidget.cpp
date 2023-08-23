#include "totalswidget.h"
#include "QtWidgets/qlineedit.h"
#include "src/database/database.h"
#include "src/gui/verification/timeinput.h"
#include "src/opl.h"
#include "src/functions/time.h"
#include "src/database/row.h"
#include "ui_totalswidget.h"

TotalsWidget::TotalsWidget(WidgetType widgetType, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TotalsWidget)
{
    ui->setupUi(this);
    setup(widgetType);
}

TotalsWidget::~TotalsWidget()
{
    delete ui;
}

/*!
 * \brief HomeWidget::fillTotals Retreives a Database Summary of Total Flight Time via the OPL::Statistics::totals
 * function and parses the return to fill out the QLineEdits.
 */
void TotalsWidget::fillTotals(WidgetType widgetType)
{
    OPL::RowData_T time_data;

    // retreive times from database
    switch (widgetType) {
    case TotalTimeWidget:
        time_data = DB->getTotals();
        break;
    case PreviousExperienceWidget:
        time_data = DB->getRowData(OPL::DbTable::Flights, TOTALS_DATA_ROW_ID);
        DEB << time_data;
    }

    // fill the line edits with the data obtained
    const OPL::RowData_T &const_time_data = qAsConst(time_data);
    for (const auto &field : const_time_data) {
        // match the db entries to the line edits using their object name
        const QString search_term = time_data.key(field) + QLatin1String("LineEdit");
        QLineEdit* line_edit = this->findChild<QLineEdit *>(search_term);
        // fill the line edit with the corresponding data
        if(line_edit != nullptr) {
            const QString &le_name = line_edit->objectName();
            if(le_name.contains("to") || le_name.contains("ldg")) {
                // line edits for take offs and landings
                line_edit->setText(field.toString());
//                DEB << "Setting: " + le_name + ": " + field.toString();
            } else {
                // line edits for total times
                const QString time_string = OPL::Time::toString(field.toInt());
                line_edit->setText(time_string);
//                DEB << "Setting " + le_name + ": " + time_string;
            }
        }

    }
}

/*!
 * \brief TotalsWidget::setup Sets the line edits as editable or read-only and connects signals if required
 * \details This widget can be used to either display the totals (in the home widget) or
 * to edit the total previous experience, from previous logbooks (in the settings widget).
 */
void TotalsWidget::setup(WidgetType widgetType)
{

    const auto lineEdits = this->findChildren<QLineEdit *>();

    switch (widgetType) {
    case TotalTimeWidget:
        LOG << "Setting up totals widget";
        for (const auto &lineEdit : lineEdits) {
            lineEdit->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        }
        fillTotals(widgetType);
        break;
    case PreviousExperienceWidget:
        LOG << "Setting up previous XP widget";
        for (const auto &lineEdit : lineEdits) {
            lineEdit->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
            lineEdit->setValidator(m_timeValidator);
        }
        // initialise m_rowData
        m_rowData = DB->getRowData(OPL::DbTable::Flights, TOTALS_DATA_ROW_ID);

        // fill the totals
        fillTotals(widgetType);
        connectSignalsAndSlots();
        break;
    default:
        break;
    }
}

/*!
 * \brief TotalsWidget::connectSignalsAndSlots If the widget is editable, connects the signals and slots
 */
void TotalsWidget::connectSignalsAndSlots()
{
    // connect signals and slots that edit the applicable field in the database on editing finished
    connect(ui->tblkLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::totalTimeEdited);
    connect(ui->tSPSELineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::spseTimeEdited);
    connect(ui->tSPMELineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::spmeTimeEdited);
    connect(ui->tMPLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::multipilotTimeEdited);

    connect(ui->tPICLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::picTimeEdited);
    connect(ui->tSICLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::sicTimeEdited);
    connect(ui->tDUALLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::dualTimeEdited);
    connect(ui->tFILineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::fiTimeEdited);
    connect(ui->tPICUSLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::picusTimeEdited);
    connect(ui->tIFRLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::ifrTimeEdited);
    connect(ui->tSIMLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::simulatorTimeEdited);

    connect(ui->toDayLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::toDayEdited);
    connect(ui->toNightLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::toNightedited);
    connect(ui->ldgDayLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::ldgDayEdited);
    connect(ui->ldgNightLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::ldgNightEdited);
}

bool TotalsWidget::verifyUserTimeInput(QLineEdit *line_edit, const TimeInput &input)
{
    if(!input.isValid()) {
        // try to fix
        QString fixed = input.fixup();
        if(fixed == QString()) {
            WARN(tr("Invalid input. Please use the following format for time:<br><br><tt>hh:mm</tt>"));
            return false;
        } else {
            line_edit->setText(fixed);
            return true;
        }
    }
    return true;
}

bool TotalsWidget::updateDatabase(const QString &db_field, const QString &value) {
    m_rowData.insert(db_field, value);
    const OPL::FlightEntry entry = OPL::FlightEntry(TOTALS_DATA_ROW_ID, m_rowData);

    return DB->commit(entry);
}

void TotalsWidget::totalTimeEdited()
{
    LOG << sender()->objectName() + "Editing finished.";
    QLineEdit* line_edit = this->findChild<QLineEdit*>(sender()->objectName());
    if (verifyUserTimeInput(line_edit, TimeInput(line_edit->text()))) {
        // write to DB
    } else {
        // (re-) set to previous value from DB (or 0)
    }
}

void TotalsWidget::spseTimeEdited()
{
    LOG << sender()->objectName() + "Editing finished.";
    QLineEdit* line_edit = this->findChild<QLineEdit*>(sender()->objectName());
    if (verifyUserTimeInput(line_edit, TimeInput(line_edit->text()))) {
        // write to DB
    } else {
        // (re-) set to previous value from DB (or 0)
    }
}

void TotalsWidget::spmeTimeEdited()
{
    LOG << sender()->objectName() + "Editing finished.";
    QLineEdit* line_edit = this->findChild<QLineEdit*>(sender()->objectName());
    if (verifyUserTimeInput(line_edit, TimeInput(line_edit->text()))) {
        // write to DB
    } else {
        // (re-) set to previous value from DB (or 0)
    }
}

void TotalsWidget::multipilotTimeEdited()
{
    LOG << sender()->objectName() + "Editing finished.";
    QLineEdit* line_edit = this->findChild<QLineEdit*>(sender()->objectName());
    if (verifyUserTimeInput(line_edit, TimeInput(line_edit->text()))) {
        // write to DB
    } else {
        // (re-) set to previous value from DB (or 0)
    }
}

void TotalsWidget::picTimeEdited()
{
    LOG << sender()->objectName() + "Editing finished.";
    QLineEdit* line_edit = this->findChild<QLineEdit*>(sender()->objectName());
    if (verifyUserTimeInput(line_edit, TimeInput(line_edit->text()))) {
        // write to DB
    } else {
        // (re-) set to previous value from DB (or 0)
    }
}

void TotalsWidget::sicTimeEdited()
{
    LOG << sender()->objectName() + "Editing finished.";
    QLineEdit* line_edit = this->findChild<QLineEdit*>(sender()->objectName());
    if (verifyUserTimeInput(line_edit, TimeInput(line_edit->text()))) {
        // write to DB
    } else {
        // (re-) set to previous value from DB (or 0)
    }
}

void TotalsWidget::dualTimeEdited()
{
    LOG << sender()->objectName() + "Editing finished.";
    QLineEdit* line_edit = this->findChild<QLineEdit*>(sender()->objectName());
    if (verifyUserTimeInput(line_edit, TimeInput(line_edit->text()))) {
        // write to DB
    } else {
        // (re-) set to previous value from DB (or 0)
    }
}

void TotalsWidget::fiTimeEdited()
{
    LOG << sender()->objectName() + "Editing finished.";
    QLineEdit* line_edit = this->findChild<QLineEdit*>(sender()->objectName());
    if (verifyUserTimeInput(line_edit, TimeInput(line_edit->text()))) {
        // write to DB
    } else {
        // (re-) set to previous value from DB (or 0)
    }
}

void TotalsWidget::picusTimeEdited()
{
    LOG << sender()->objectName() + "Editing finished.";
    QLineEdit* line_edit = this->findChild<QLineEdit*>(sender()->objectName());
    if (verifyUserTimeInput(line_edit, TimeInput(line_edit->text()))) {
        // write to DB
    } else {
        // (re-) set to previous value from DB (or 0)
    }
}

void TotalsWidget::ifrTimeEdited()
{
    LOG << sender()->objectName() + "Editing finished.";
    QLineEdit* line_edit = this->findChild<QLineEdit*>(sender()->objectName());
    if (verifyUserTimeInput(line_edit, TimeInput(line_edit->text()))) {
        // write to DB
    } else {
        // (re-) set to previous value from DB (or 0)
    }
}

void TotalsWidget::nightTimeEdited()
{
    LOG << sender()->objectName() + "Editing finished.";
    QLineEdit* line_edit = this->findChild<QLineEdit*>(sender()->objectName());
    if (verifyUserTimeInput(line_edit, TimeInput(line_edit->text()))) {
        // write to DB
    } else {
        // (re-) set to previous value from DB (or 0)
    }
}

void TotalsWidget::simulatorTimeEdited()
{
    LOG << sender()->objectName() + "Editing finished.";
    QLineEdit* line_edit = this->findChild<QLineEdit*>(sender()->objectName());
    if (verifyUserTimeInput(line_edit, TimeInput(line_edit->text()))) {
        // write to DB
    } else {
        // (re-) set to previous value from DB (or 0)
    }
}

void TotalsWidget::toDayEdited()
{
    LOG << sender()->objectName() + "Editing finished.";
}

void TotalsWidget::toNightedited()
{
    LOG << sender()->objectName() + "Editing finished.";
}

void TotalsWidget::ldgDayEdited()
{
    LOG << sender()->objectName() + "Editing finished.";
}

void TotalsWidget::ldgNightEdited()
{
    LOG << sender()->objectName() + "Editing finished.";
}


