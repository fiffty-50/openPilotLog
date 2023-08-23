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
void TotalsWidget::fillTotals(const WidgetType widgetType)
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
            } else {
                // line edits for total times
                const QString time_string = OPL::Time::toString(field.toInt());
                line_edit->setText(time_string);
            }
        }

    }
}

/*!
 * \brief TotalsWidget::setup Sets the line edits as editable or read-only and connects signals if required
 * \details This widget can be used to either display the totals (in the home widget) or
 * to edit the total previous experience, from previous logbooks (in the settings widget).
 */
void TotalsWidget::setup(const WidgetType widgetType)
{
    const QList<QLineEdit *> lineEdits = this->findChildren<QLineEdit *>();

    switch (widgetType) {
    case TotalTimeWidget:
        LOG << "Setting up totals widget";
        // disable editing
        for (const auto &lineEdit : lineEdits) {
            lineEdit->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        }
        // populate the UI
        fillTotals(widgetType);
        break;
    case PreviousExperienceWidget:
        LOG << "Setting up previous XP widget";
        for (const auto &lineEdit : lineEdits) {
            lineEdit->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
            // set a validator for the TO/LDG line edits, the other ones get validated seperately
            if(lineEdit->objectName().contains(QLatin1String("to")) || lineEdit->objectName().contains(QLatin1String("ldg"))) {
                lineEdit->setValidator( new QIntValidator(0,  std::numeric_limits<int>::max(), this) );
            }
        }
        // initialise m_rowData
        m_rowData = DB->getRowData(OPL::DbTable::Flights, TOTALS_DATA_ROW_ID);

        // populate the UI
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
    connect(ui->tblkLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::timeLineEditEditingFinished);
    connect(ui->tSPSELineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::timeLineEditEditingFinished);
    connect(ui->tSPMELineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::timeLineEditEditingFinished);
    connect(ui->tMPLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::timeLineEditEditingFinished);

    connect(ui->tPICLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::timeLineEditEditingFinished);
    connect(ui->tSICLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::timeLineEditEditingFinished);
    connect(ui->tDUALLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::timeLineEditEditingFinished);
    connect(ui->tFILineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::timeLineEditEditingFinished);
    connect(ui->tPICUSLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::timeLineEditEditingFinished);
    connect(ui->tIFRLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::timeLineEditEditingFinished);
    connect(ui->tNIGHTLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::timeLineEditEditingFinished);
    connect(ui->tSIMLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::timeLineEditEditingFinished);

    connect(ui->toDayLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::movementLineEditEditingFinished);
    connect(ui->toNightLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::movementLineEditEditingFinished);
    connect(ui->ldgDayLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::movementLineEditEditingFinished);
    connect(ui->ldgNightLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::movementLineEditEditingFinished);
}

/*!
 * \brief TotalsWidget::verifyUserTimeInput verify the user input is correct or can be fixed
 * \param line_edit the line edit that has been edited
 * \param input the user input
 * \return if the input is valid or can be fixed
 */
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

/*!
 * \brief TotalsWidget::updateTimeEntry Updates the DB with a time entry
 * \param line_edit The time line edit that has been edited
 * \return true on success
 */
bool TotalsWidget::updateTimeEntry(const QLineEdit* line_edit) {
    const QString db_field = line_edit->objectName().remove(QLatin1String("LineEdit"));
    const QVariant value = OPL::Time::toMinutes(line_edit->text());

    m_rowData.insert(db_field, value);

    const auto previous_experience = OPL::FlightEntry(TOTALS_DATA_ROW_ID, m_rowData);
    return DB->commit(previous_experience);
}

/*!
 * \brief TotalsWidget::updateMovementEntry Updates the DB with a movement (TO or LDG) entry
 * \param line_edit The line edit that has been edited
 * \return true on success
 */
bool TotalsWidget::updateMovementEntry(const QLineEdit *line_edit)
{
    const QString db_field = line_edit->objectName().remove(QLatin1String("LineEdit"));
    const QVariant value = line_edit->text().toInt();

    m_rowData.insert(db_field, value);

    const auto previous_experience = OPL::FlightEntry(TOTALS_DATA_ROW_ID, m_rowData);
    return DB->commit(previous_experience);
}

void TotalsWidget::timeLineEditEditingFinished()
{
    LOG << sender()->objectName() + "Editing finished.";
    QLineEdit* line_edit = this->findChild<QLineEdit*>(sender()->objectName());

    // verify and if possible fix the user input
    if (!verifyUserTimeInput(line_edit, TimeInput(line_edit->text()))) {
        // (re-) set to previous value from DB
        WARN(tr("Invalid time entry. Please use the following format:<br><br><tt> hh:mm </tt>"));
        int old_value = m_rowData.value(line_edit->objectName().remove(QLatin1String("LineEdit"))).toInt();
        line_edit->setText(OPL::Time::toString(old_value));
        return;
    }

    // write the updated value to the database
    updateTimeEntry(line_edit);

}

void TotalsWidget::movementLineEditEditingFinished()
{
    // input validation is already done by the QValidator
    LOG << sender()->objectName() + "Editing finished.";
    updateMovementEntry(this->findChild<QLineEdit*>(sender()->objectName()));
}

