#include "totalswidget.h"
#include "QtWidgets/qlineedit.h"
#include "src/functions/statistics.h"
#include "src/opl.h"
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
    switch (widgetType) {
    case TotalTimeWidget:
    {
        const auto data = OPL::Statistics::totals();
        for (const auto &field : data) {
            auto line_edit = this->findChild<QLineEdit *>(field.first + QLatin1String("LineEdit"));
            line_edit->setText(field.second);
        }
        break;
    }
    case PreviousExperienceWidget:
        WARN("NOT IMPLEMENTED!");
        qApp->quit();
        // get previous exp data and fill
        // implement query in opl statistics
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
        for (const auto &lineEdit : lineEdits) {
            lineEdit->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        }
        fillTotals(widgetType);
        break;
    case PreviousExperienceWidget:
        for (const auto &lineEdit : lineEdits) {
            lineEdit->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
        }
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
    connect(ui->totalLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::totalTimeEdited);
    connect(ui->spseLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::spseTimeEdited);
    connect(ui->spmeLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::spmeTimeEdited);
    connect(ui->multipilotLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::multipilotTimeEdited);

    connect(ui->picLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::picTimeEdited);
    connect(ui->sicLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::sicTimeEdited);
    connect(ui->dualLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::dualTimeEdited);
    connect(ui->fiLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::fiTimeEdited);
    connect(ui->picusLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::picusTimeEdited);
    connect(ui->ifrLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::ifrTimeEdited);
    connect(ui->simLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::simulatorTimeEdited);

    connect(ui->todayLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::toDayEdited);
    connect(ui->tonightLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::toNightedited);
    connect(ui->ldgdayLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::ldgDayEdited);
    connect(ui->ldgnightLineEdit, &QLineEdit::editingFinished,
            this, &TotalsWidget::ldgNightEdited);
}

void TotalsWidget::totalTimeEdited()
{
    INFO("Total Time Edited.");
}

void TotalsWidget::spseTimeEdited()
{

}

void TotalsWidget::spmeTimeEdited()
{

}

void TotalsWidget::multipilotTimeEdited()
{

}

void TotalsWidget::picTimeEdited()
{

}

void TotalsWidget::sicTimeEdited()
{

}

void TotalsWidget::dualTimeEdited()
{

}

void TotalsWidget::fiTimeEdited()
{

}

void TotalsWidget::picusTimeEdited()
{

}

void TotalsWidget::ifrTimeEdited()
{

}

void TotalsWidget::nightTimeEdited()
{

}

void TotalsWidget::simulatorTimeEdited()
{

}

void TotalsWidget::toDayEdited()
{

}

void TotalsWidget::toNightedited()
{

}

void TotalsWidget::ldgDayEdited()
{

}

void TotalsWidget::ldgNightEdited()
{

}


