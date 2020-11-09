/*
 *openPilot Log - A FOSS Pilot Logbook Application
 *Copyright (C) 2020  Felix Turowsky
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "aircraftwidget.h"
#include "ui_aircraftwidget.h"

// Debug Makro
#define DEB(expr) \
    qDebug() << __PRETTY_FUNCTION__ << "\t" << expr

AircraftWidget::AircraftWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AircraftWidget)
{
    ui->setupUi(this);
    refreshView();
}

AircraftWidget::~AircraftWidget()
{
    delete ui;
}

void AircraftWidget::setSelectedAircraft(const qint32 &value)
{
    selectedAircraft = value;
}

void AircraftWidget::tableView_selectionChanged(const QItemSelection &index, const QItemSelection &)
{
    setSelectedAircraft(index.indexes()[0].data().toInt());
    DEB("Selected aircraft with ID#: " << selectedAircraft);

    auto nt = new NewTail(Aircraft(selectedAircraft), Db::editExisting, this);
    connect(nt, SIGNAL(accepted()), this, SLOT(acft_accepted()));
    connect(nt, SIGNAL(rejected()), this, SLOT(acft_accepted()));

    nt->setWindowFlag(Qt::Widget);
    ui->stackedWidget->addWidget(nt);
    ui->stackedWidget->setCurrentWidget(nt);
}

void AircraftWidget::on_deleteButton_clicked()
{
    if (selectedAircraft > 0) {

        auto ac = new Aircraft(selectedAircraft);
        if(!ac->remove()) {
            QVector<QString> columns = {"doft","dept","dest"};
            QVector<QString> details = Db::multiSelect(columns, "flights", "acft",
                                                       QString::number(selectedAircraft), Db::exactMatch);
            auto mb = new QMessageBox(this);
            QString message = "\nUnable to delete. The following error has ocurred:\n\n";
            if(!details.isEmpty()){
                message += ac->error + QLatin1String("\n\n");
                message += "This is most likely the case because a flight exists with the aircaft "
                           "you are trying to delete. You have to change or remove this flight "
                           "before being able to remove this aircraft.\n\n"
                           "The following flight(s) with this tail have been found:\n\n";
                auto space = QLatin1Char(' ');
                for(int i = 0; i <= 30 && i <=details.length()-3; i+=3){
                    message += details[i] + space
                             + details[i+1] + space
                             + details[i+2] + QLatin1String("\n");
                }
            }
            mb->setText(message);
            mb->setIcon(QMessageBox::Critical);
            mb->show();
        }
        refreshView();

    } else {
        auto mb = new QMessageBox(this);
        mb->setText("No aircraft selected.");
        mb->show();
    }
}

void AircraftWidget::on_newButton_clicked()
{
    auto nt = new NewTail(QString(), Db::createNew, this);
    connect(nt,
            SIGNAL(accepted()), this,
            SLOT(acft_accepted()));
    nt->show();
}

void AircraftWidget::acft_accepted()
{
    DEB("Refreshing View...");
    refreshView();
}

void AircraftWidget::refreshView()
{
    QString welcomeMessage = "Select an Aircraft to show or edit details.";
    QWidget *start = new QWidget();
    start->setObjectName("welcomeAC");
    QLabel *label = new QLabel(welcomeMessage);
    label->setAlignment(Qt::AlignCenter);
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(label);
    start->setLayout(layout);
    ui->stackedWidget->addWidget(start);
    ui->stackedWidget->setCurrentWidget(start);

    QSqlTableModel *model = new QSqlTableModel;
    model->setTable("viewTails");
    model->select();

    QTableView *view = ui->tableView;
    view->setModel(model);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    view->setColumnWidth(0, 60);
    view->setColumnWidth(1, 120);
    view->setColumnWidth(2, 180);
    view->verticalHeader()->hide();
    view->setAlternatingRowColors(true);
    view->setSortingEnabled(true);

    view->sortByColumn(Settings::read("userdata/acSortColumn").toInt(), Qt::AscendingOrder);

    view->show();

    connect(ui->tableView->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            SLOT(tableView_selectionChanged(const QItemSelection &, const QItemSelection &)));
}
