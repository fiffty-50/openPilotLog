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
#include "debug.h"

PilotsWidget::PilotsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PilotsWidget)
{
    ui->setupUi(this);
    sortColumn = Settings::read("userdata/pilSortColumn").toInt();
    refreshModelAndView();         
}

PilotsWidget::~PilotsWidget()
{
    delete ui;
}

void PilotsWidget::tableView_selectionChanged()//const QItemSelection &index, const QItemSelection &
{   
    auto *selection = ui->tableView->selectionModel();
    selectedPilots.clear();

    for (const auto& row : selection->selectedRows()) {
        selectedPilots << row.data().toInt();
        DEB("Selected Tails(s) with ID: " << selectedPilots);
    }
    if(selectedPilots.length() == 1) {

        NewPilotDialog* np = new NewPilotDialog(Pilot(selectedPilots.first()), Db::editExisting, this);
        connect(np, SIGNAL(accepted()), this, SLOT(pilot_editing_finished()));
        connect(np, SIGNAL(rejected()), this, SLOT(pilot_editing_finished()));
        np->setWindowFlag(Qt::Widget);
        np->setAttribute(Qt::WA_DeleteOnClose);
        ui->stackedWidget->addWidget(np);
        ui->stackedWidget->setCurrentWidget(np);
        np->exec();
    }
}

void PilotsWidget::tableView_headerClicked(int column)
{
    sortColumn = column;
    Settings::write("userdata/pilSortColumn", column);
}

void PilotsWidget::on_newButton_clicked()
{
    NewPilotDialog* np = new NewPilotDialog(Db::createNew, this);
    np->setAttribute(Qt::WA_DeleteOnClose);
    connect(np, SIGNAL(accepted()),
            this, SLOT(pilot_editing_finished()));
    np->exec();
}

void PilotsWidget::on_deletePushButton_clicked()
{
    if(selectedPilots.length() == 1){
        for(const auto& selectedPilot : selectedPilots){
            if (selectedPilot > 0) {
                auto pil = Pilot(selectedPilot);
                if(!pil.remove()) {
                    QVector<QString> columns = {"doft","dept","dest"};
                    QVector<QString> details = Db::multiSelect(columns, "flights", "pic",
                                                               QString::number(selectedPilot), Db::exactMatch);
                    auto mb = QMessageBox(this);
                    QString message = "\nUnable to delete. The following error has ocurred:\n\n";
                    if(!details.isEmpty()){
                        message += pil.error + QLatin1String("\n\n");
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
                    mb.setText(message);
                    mb.setIcon(QMessageBox::Critical);
                    mb.exec();
                }
            }
        }
        refreshModelAndView();
    } else {
        auto mb = QMessageBox(this);
        mb.setText("No Pilot selected.");
        mb.show();
    }
}

void PilotsWidget::pilot_editing_finished()
{
    refreshModelAndView();
}

void PilotsWidget::refreshModelAndView()
{
    ui->stackedWidget->addWidget(parent()->findChild<QWidget*>("welcomePage"));
    ui->stackedWidget->setCurrentWidget(parent()->findChild<QWidget*>("welcomePage"));

    model->setTable("viewPilots");
    model->setFilter("ID > 1");//to not allow editing of self, shall be done via settings
    model->select();

    QTableView *view = ui->tableView;
    view->setModel(model);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    view->hideColumn(0);
    view->setColumnWidth(1, 180);
    view->setColumnWidth(2, 180);
    view->verticalHeader()->hide();
    view->setAlternatingRowColors(true);
    view->setSortingEnabled(true);

    view->sortByColumn(sortColumn, Qt::AscendingOrder);

    view->show();

    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(tableView_selectionChanged()));
    connect(ui->tableView->horizontalHeader(), SIGNAL(sectionClicked(int)),
            this, SLOT(tableView_headerClicked(int)));
}

void PilotsWidget::on_searchLineEdit_textChanged(const QString &arg1)
{
    model->setFilter("\"" + ui->searchComboBox->currentText() + "\" LIKE \"%" + arg1 + "%\" AND ID > 1");
}
