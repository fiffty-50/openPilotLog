#include "airportwidget.h"
#include "ui_airportwidget.h"
#include "src/gui/dialogues/newairportdialog.h"

AirportWidget::AirportWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AirportWidget)
{
    ui->setupUi(this);
    setupModelAndeView();
    setupSearch();
}

AirportWidget::~AirportWidget()
{
    delete ui;
}

void AirportWidget::setupModelAndeView()
{
    model = new QSqlTableModel(this);
    model->setTable(TABLE_NAME);
    model->select();

    view = ui->tableView;
    view->setModel(model);

    // To Do: Decide on and implement editing behaviour
    //view->setSelectionBehavior(QAbstractItemView::SelectRows);
    //view->setSelectionMode(QAbstractItemView::SingleSelection);
    //view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    view->hideColumn(0);
    view->resizeColumnsToContents();
    view->verticalHeader()->hide();
    view->setAlternatingRowColors(true);
    view->show();
}

void AirportWidget::setupSearch()
{
    for (const auto &item : HEADER_MAP)
        ui->searchComboBox->addItem(item);
}

void AirportWidget::on_searchLineEdit_textChanged(const QString &arg1)
{
    if(arg1.length() == 0) {
        model->setFilter("");
        model->select();
        return;
    }

    model->setFilter(FILTER_MAP.value(ui->searchComboBox->currentIndex())
                     + arg1 + QLatin1String("%\""));
}


void AirportWidget::on_searchComboBox_currentIndexChanged(int index)
{
    ui->searchLineEdit->setText(QString());
}

void AirportWidget::on_newAirportPushButton_clicked()
{
    auto ap_dialog = NewAirportDialog(this);
    if (ap_dialog.exec() == QDialog::Accepted)
        model->select();
}
