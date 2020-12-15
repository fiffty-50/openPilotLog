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
    DEB("New AircraftWidet");
    ui->setupUi(this);
    ui->stackedWidget->addWidget(this->findChild<QWidget*>("welcomePageTails"));
    ui->stackedWidget->setCurrentWidget(this->findChild<QWidget*>("welcomePageTails"));

    setupModelAndView();
}

AircraftWidget::~AircraftWidget()
{
    DEB("Deleting NewAircraftWidget");
    delete ui;
}

/*
 * Functions
 */

void AircraftWidget::setupModelAndView()
{
    sortColumn = ASettings::read("userdata/acSortColumn").toInt();

    model = new QSqlTableModel(this);
    model->setTable("viewTails");
    model->select();

    view = ui->tableView;
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
    view->sortByColumn(sortColumn, Qt::DescendingOrder);

    view->show();
    selection = view->selectionModel();

    QObject::connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
                     this, &AircraftWidget::tableView_selectionChanged);
    QObject::connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionClicked,
                     this, &AircraftWidget::tableView_headerClicked);
}

/*
 * Slots
 */
void AircraftWidget::on_deleteButton_clicked()
{
    if (selectedTails.length() > 0) {
        for(const auto& selectedTail : selectedTails){
            auto ac = Aircraft(selectedTail);
            auto& warningMsg = "Deleting Aircraft with registration:<br><center><b>"
                    + ac.data.value("registration")
                    + "<br></center></b>Do you want to proceed?";
            QMessageBox confirm;
            confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            confirm.setDefaultButton(QMessageBox::No);
            confirm.setIcon(QMessageBox::Question);
            confirm.setWindowTitle("Delete Aircraft");
            confirm.setText(warningMsg);
            int reply = confirm.exec();
            if(reply == QMessageBox::Yes) {
                if(!ac.remove()) {
                    QVector<QString> columns = {"doft","dept","dest"};
                    QVector<QString> details = Db::multiSelect(columns, "flights", "acft",
                                                               QString::number(selectedTail), Db::exactMatch);
                    auto mb = QMessageBox(this);
                    QString message = "\nUnable to delete. The following error has ocurred:\n\n";
                    if(!details.isEmpty()){
                        message += ac.error + QLatin1String("\n\n");
                        message += "This is most likely the case because a flight exists with the aircaft "
                                   "you are trying to delete. In order to maintain database integrity, you have to\n"
                                   "change or remove this flight before being able to remove this aircraft.\n\n"
                                   "The following flight(s) with this tail have been found:\n\n";
                        auto space = QLatin1Char(' ');
                        for(int i = 0; i <= 30 && i <=details.length()-3; i+=3){
                            message += details[i] + space
                                     + details[i+1] + space
                                     + details[i+2] + QLatin1String("\n");
                        }
                    }
                    mb.setText(message);
                    mb.setIcon(QMessageBox::Critical);
                    mb.show();
                }
            }
        }
        model->select();
    } else {
        auto mb = QMessageBox(this);
        mb.setText("No aircraft selected.");
        mb.show();
    }
}

void AircraftWidget::on_newButton_clicked()
{
    auto nt = NewTailDialog(QString(), this);
    connect(&nt, SIGNAL(accepted()), this, SLOT(acft_editing_finished()));
    connect(&nt, SIGNAL(rejected()), this, SLOT(acft_editing_finished()));
    nt.exec();
}

void AircraftWidget::on_aircraftSearchLineEdit_textChanged(const QString &arg1)
{
    if(ui->aircraftSearchComboBox->currentIndex() == 0){
        ui->aircraftSearchLineEdit->setText(arg1.toUpper());
    }
    model->setFilter(ui->aircraftSearchComboBox->currentText() + " LIKE \"%" + arg1 + "%\"");
}

void AircraftWidget::tableView_selectionChanged()
{
    if (this->findChild<NewTailDialog*>() != nullptr) {
        DEB("Selection changed. Deleting orphaned dialog.");
        delete this->findChild<NewTailDialog*>();
        /// [F] if the user changes the selection without making any changes,
        /// if(selectedTails.length() == 1) spawns a new dialog without the
        /// old one being deleted, since neither accept() nor reject() was emitted.
        /// Is this a reasonable way of avoiding pollution? Creating the widgets on the
        /// stack does not seem to solve the problem since the Dialog does not get destroyed
        /// until either accept() or reject() is emitted so I went for this solution.
    }
    selectedTails.clear();

    for (const auto& row : selection->selectedRows()) {
        selectedTails << row.data().toInt();
        DEB("Selected Tails(s) with ID: " << selectedTails);
    }
    if(selectedTails.length() == 1) {
        auto* nt = new NewTailDialog(selectedTails.first(), this);
        QObject::connect(nt,   &QDialog::accepted,
                         this, &AircraftWidget::acft_editing_finished);
        QObject::connect(nt,   &QDialog::rejected,
                         this, &AircraftWidget::acft_editing_finished);
        ui->stackedWidget->addWidget(nt);
        ui->stackedWidget->setCurrentWidget(nt);
        nt->setWindowFlag(Qt::Widget);
        nt->setAttribute(Qt::WA_DeleteOnClose);
        nt->exec();
    }
}

void AircraftWidget::tableView_headerClicked(int column)
{
    sortColumn = column;
    ASettings::write("userdata/acSortColumn", column);
}

void AircraftWidget::acft_editing_finished()
{
    model->select();
}
