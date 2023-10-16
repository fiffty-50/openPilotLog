#include "totalswidget.h"
#include "QtWidgets/qlineedit.h"
#include "src/database/database.h"
#include "src/opl.h"
#include "src/classes/time.h"
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
                lineEdit->setValidator(new QIntValidator(0, std::numeric_limits<int>::max(), this));
            }
        }
        // initialise m_rowData
        m_rowData = DB->getRowData(OPL::DbTable::PreviousExperience, ROW_ID);

        // populate the UI
        fillTotals(widgetType);
        connectSignalsAndSlots();
        break;
    default:
        break;
    }
}

/*!
 * \brief HomeWidget::fillTotals Retreives a Database Summary of Total Flight Time and fills the UI.
 */
void TotalsWidget::fillTotals(const WidgetType widgetType)
{
    OPL::RowData_T time_data;

    // retreive times from database
    switch (widgetType) {
    case TotalTimeWidget:
        time_data = DB->getTotals(true);
        break;
    case PreviousExperienceWidget:
        time_data = DB->getRowData(OPL::DbTable::PreviousExperience, ROW_ID);
        LOG << "Previous row data:";
        LOG << time_data;
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
                DEB << "Setting to/ldg: " << le_name << " -> " + field.toString();
                line_edit->setText(field.toString());
            } else {
                // line edits for total time
                OPL::Time time = OPL::Time(field.toInt());// = Time(field.toInt());
                line_edit->setText(time.toString());
            }
        }

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


void TotalsWidget::timeLineEditEditingFinished()
{
    LOG << sender()->objectName() + "Editing finished.";
    QLineEdit* line_edit = this->findChild<QLineEdit*>(sender()->objectName());
    const QString& text = line_edit->text();

    // make sure the input is usable
    if(!text.contains(QChar(':'))) {
        WARN(tr("Please enter the time as: <br><br> hh:mm"));
        line_edit->setText(QString());
        return;
    }

    // write the updated value to the database
    const QString db_field = line_edit->objectName().remove(QLatin1String("LineEdit"));
    const QVariant value = OPL::Time::fromString(line_edit->text()).toMinutes();

    m_rowData.insert(db_field, value);
    LOG << "Added row data: " + db_field + ": " + value.toString();

    const auto previous_experience = OPL::PreviousExperienceEntry(ROW_ID, m_rowData);
    DB->commit(previous_experience);

    // Read back the value and set the line edit to confirm input is correct and provide user feedback
    m_rowData = DB->getRowData(OPL::DbTable::PreviousExperience, ROW_ID);
    OPL::Time new_time = OPL::Time(m_rowData.value(db_field).toInt());
    line_edit->setText(new_time.toString());
}

void TotalsWidget::movementLineEditEditingFinished()
{
    // input validation is done by the QValidator
    QLineEdit* line_edit = this->findChild<QLineEdit*>(sender()->objectName());
    LOG << line_edit->objectName() + "Editing finished.";

    // extract the value from the input and update the DB
    const QString db_field = line_edit->objectName().remove(QLatin1String("LineEdit"));
    const QVariant value = line_edit->text().toInt();

    m_rowData.insert(db_field, value);

    const auto previous_experience = OPL::PreviousExperienceEntry(ROW_ID, m_rowData);
    DB->commit(previous_experience);

    // read back the value and set the line edit to the retreived value to give user feedback
    m_rowData = DB->getRowData(OPL::DbTable::PreviousExperience, ROW_ID);
    const QString new_value = QString::number(m_rowData.value(db_field).toInt());
    line_edit->setText(new_value);
}

