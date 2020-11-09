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
#include "pilotswidget.h"
#include "ui_pilotswidget.h"

// Debug Makro
#define DEB(expr) \
    qDebug() << __PRETTY_FUNCTION__ << "\t" << expr

PilotsWidget::PilotsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PilotsWidget)
{
    ui->setupUi(this);
    refreshView();
}

PilotsWidget::~PilotsWidget()
{
    delete ui;
}

void PilotsWidget::tableView_selectionChanged(const QItemSelection &index, const QItemSelection &)
{
    setSelectedPilot(index.indexes()[0].data().toInt());
    DEB("Selected Pilot with ID#: " << selectedPilot);

    auto np = new NewPilot(Pilot(selectedPilot), Db::editExisting, this);
    connect(np, SIGNAL(accepted()), this, SLOT(widget_accepted()));
    connect(np, SIGNAL(rejected()), this, SLOT(widget_accepted()));

    np->setWindowFlag(Qt::Widget);
    ui->stackedWidget->addWidget(np);
    ui->stackedWidget->setCurrentWidget(np);

}

void PilotsWidget::setSelectedPilot(const qint32 &value)
{
    selectedPilot = value;
}

void PilotsWidget::on_newButton_clicked()
{
    auto np = new NewPilot(Db::createNew, this);
    connect(np,
            SIGNAL(accepted()), this,
            SLOT(widget_accepted()));
    np->show();
}

void PilotsWidget::on_deletePushButton_clicked()
{
    if (selectedPilot > 0) {

        auto pil = new Pilot(selectedPilot);
        if(!pil->remove()) {
            QVector<QString> columns = {"doft","dept","dest"};
            QVector<QString> details = Db::multiSelect(columns, "flights", "pic",
                                                       QString::number(selectedPilot), Db::exactMatch);
            auto mb = new QMessageBox(this);
            QString message = "\nUnable to delete. The following error has ocurred:\n\n";
            if(!details.isEmpty()){
                message += pil->error + QLatin1String("\n\n");
                message += "This is most likely the case because a flight exists with the Pilot "
                           "you are trying to delete. You have to change or remove this flight "
                           "before being able to remove this pilot from the database.\n\n"
                           "The following flight(s) with this pilot have been found:\n\n";
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
        mb->setText("No Pilot selected.");
        mb->show();
    }
}

void PilotsWidget::widget_accepted()
{
    DEB("Refreshing View...");
    refreshView();
}

void PilotsWidget::refreshView()
{
    QString welcomeMessage = "Select a Pilot to show or edit details.";
    QWidget *start = new QWidget();
    start->setObjectName("welcomePL");
    QLabel *label = new QLabel(welcomeMessage);
    label->setAlignment(Qt::AlignCenter);
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(label);
    start->setLayout(layout);
    ui->stackedWidget->addWidget(start);
    ui->stackedWidget->setCurrentWidget(start);

    QSqlTableModel *model = new QSqlTableModel;
    model->setTable("viewPilots");
    model->setFilter("ID > 1");//to not allow editing of self, shall be done via settings
    model->select();

    QTableView *view = ui->tableView;
    view->setModel(model);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    view->setColumnWidth(0, 60);
    view->setColumnWidth(1, 180);
    view->setColumnWidth(2, 120);
    view->verticalHeader()->hide();
    view->setAlternatingRowColors(true);
    view->setSortingEnabled(true);

    view->sortByColumn(Settings::read("userdata/pilSortColumn").toInt(), Qt::AscendingOrder);

    view->show();

    connect(ui->tableView->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            SLOT(tableView_selectionChanged(const QItemSelection &, const QItemSelection &)));
}
