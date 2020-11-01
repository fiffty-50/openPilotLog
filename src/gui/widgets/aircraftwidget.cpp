#include "aircraftwidget.h"
#include "ui_aircraftwidget.h"

// Debug Makro
#define DEB(expr) \
    qDebug() << "aircraftWidget ::" << __func__ << "\t" << expr

aircraftWidget::aircraftWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::aircraftWidget)
{
    ui->setupUi(this);

    QString welcomeMessage = "Select an Aircraft to show or edit details.";
    QWidget *start = new QWidget();
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
    view->setColumnWidth(1,120);
    view->setColumnWidth(2,60);
    view->setColumnWidth(3,60);
    view->setColumnWidth(4,60);
    view->setColumnWidth(5,60);
    view->verticalHeader()->hide();
    view->setAlternatingRowColors(true);
    for (int i = 6;i <= 17; i++) {
        view->hideColumn(i);
    }
    view->setSortingEnabled(true);
    view->sortByColumn(0,Qt::AscendingOrder);
    view->show();

    connect(ui->tableView->selectionModel(),
    SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
    SLOT(tableView_selectionChanged(const QItemSelection &, const QItemSelection &)));
}

aircraftWidget::~aircraftWidget()
{
    delete ui;
}

void aircraftWidget::setSelectedAircraft(const qint32 &value)
{
    selectedAircraft = value;
}

void aircraftWidget::tableView_selectionChanged(const QItemSelection &index, const QItemSelection &)
{
    setSelectedAircraft(index.indexes()[0].data().toInt());
    DEB("Selected aircraft with ID#: " << selectedAircraft);

    auto nt = new NewTail(db(sql::tails,selectedAircraft),sql::editExisting,this);
    //auto nt = new NewTail(db(sql::tails,selectedAircraft),this);

    nt->setWindowFlag(Qt::Widget);
    ui->stackedWidget->addWidget(nt);
    ui->stackedWidget->setCurrentWidget(nt);
}

void aircraftWidget::on_deleteButton_clicked()
{
    if(selectedAircraft > 0){

        db::deleteRow("tails","_rowid_",QString::number(selectedAircraft),sql::exactMatch);

        QSqlTableModel *model = new QSqlTableModel;
        model->setTable("viewTails");
        model->select();
        ui->tableView->setModel(model);


    }else{
        auto mb = new QMessageBox(this);
        mb->setText("No aircraft selected.");
        mb->show();
    }
}
