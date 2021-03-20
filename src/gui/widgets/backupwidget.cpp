#include "backupwidget.h"
#include "ui_backupwidget.h"
#include "src/classes/astandardpaths.h"
#include "src/testing/adebug.h"
#include "src/database/adatabase.h"

#include <QListView>
#include <QStandardItemModel>
#include <QFileIconProvider>
#include <QMessageBox>
#include <QFileDialog>

BackupWidget::BackupWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BackupWidget)
{
    ui->setupUi(this);

    /* This is just a quick and dirty overview to get something into the view so you have an idea.
     * The goal is that the view does not just display file names, but a String, for example
     *
     * Available Backups
     * =================
     * 2020-01-01 - 476 Flights, 23 Aircraft, 12 Pilots, 1580:33 Total Time, Last Flight 2019-12-28
     * 2020-02-02 - 512 Flights, 25 Aircraft, 13 Pilots, 1640:47 Total Time, Last Flight 2020-01-23
     * ...
     *
     * So instead of QFileSystemModel with QListView, which would be the easiest way, using QAbstractItemModel
     * and QTableView might be preferable since it allows us to customize the data we display.
     *
     * Below is just some sample code to fill the table with data to give you an impression of what
     * the end result is supposed to be like. If you have another idea of how to get there, that's fine as well!
     *
     */


    fillTableWithSampleData();
}

BackupWidget::~BackupWidget()
{
    delete ui;
}


void BackupWidget::on_tableView_clicked(const QModelIndex &index) {
    selected = model->item(index.row(), 0);
    DEB << "Item at row:" << index.row() << "->" << selected->data(Qt::DisplayRole);
}

void BackupWidget::on_createLocalPushButton_clicked()
{
    // Copy database to backupdir
}

void BackupWidget::on_restoreLocalPushButton_clicked()
{
    // Restore the selected entry from the list
}

void BackupWidget::on_deleteSelectedPushButton_clicked()
{
    DEB << "deleting:" << selected->data(Qt::DisplayRole);
}

void BackupWidget::on_createExternalPushButton_clicked()
{
    QString filename = QFileDialog::getSaveFileName(
                this,
                "Choose destination file",
                AStandardPaths::directory(AStandardPaths::Backup).absolutePath(),
                ".db"
    );
    // [G] TODO: get time to properly format filename
    // Open something like a QFileDialog and let the user choose where to save the backup
}

void BackupWidget::on_restoreExternalPushButton_clicked()
{
    QString filename = QFileDialog::getSaveFileName(
                this,
                "Choose backup file",
                QStandardPaths::displayName(QStandardPaths::HomeLocation),
                ".db"
    );
    // Open something like a QFileDialog and let the user choose where to load the backup from
}

void BackupWidget::on_aboutPushButton_clicked() {
    // Shows a message box explaining a little about local and external backups
    // [G]: Add message text. Could this be predefined in Opl::Assets?
    QMessageBox msg_box(QMessageBox::Information, "About backups", "...", QMessageBox::Ok);
    msg_box.exec();
}



// ===================================================================================

// feel free to delete this as you go along, just here for demonstration purposes
void BackupWidget::fillTableWithSampleData()
{
    // First column in table, would be created by listing the files in backupdir
    QDir backup_dir = QDir(AStandardPaths::directory(AStandardPaths::Backup));
    QStringList entries = backup_dir.entryList(QStringList{"*.db"}, QDir::Files, QDir::Time);
    QList<QStandardItem*> filenames;
    QFileIconProvider provider;

    for(auto& entry : entries) {
        auto item = new QStandardItem(entry);
        item->setIcon(provider.icon(QFileIconProvider::File));
        filenames.append(item);
    }

    // [G]: works but a bit too hardcoded perhaps? The aviation industry wont change overnight
    // but still it could be worthwile to at least have the names a bit more encapsulated in the
    // database so we have them more "centralised" at least.

    // Get summary of each db file and populate lists (columns) of data
    QList<QStandardItem *> total_flights, total_tails, total_pilots, max_doft, total_time;
    for (const auto &entry : entries) {
        QMap<QString, QString> summary = aDB->databaseSummary(backup_dir.absoluteFilePath(entry));
        total_flights.append(new QStandardItem(summary["total_flights"]));
        total_tails.append(new QStandardItem(summary["total_tails"]));
        total_pilots.append(new QStandardItem(summary["total_pilots"]));
        max_doft.append(new QStandardItem(summary["max_doft"]));
        total_time.append(new QStandardItem(summary["total_time"]));
    }

    // [G]: Sort entries? based on what? the files are abit inconsistent in their naming atm
    // but i assume we could sort based on the time in the file name?

    model = new QStandardItemModel(this);
    model->insertColumn(0, filenames);
    model->insertColumn(1, total_flights);  // flight
    model->insertColumn(2, total_tails);  // tails
    model->insertColumn(3, total_pilots);  // pilots
    model->insertColumn(4, max_doft);  // doft
    model->insertColumn(5, total_time);  // time
    model->setHorizontalHeaderLabels(QStringList{"Backup File","Total Flights", "Total Tails",
                                                 "Total Pilots", "Max Doft", "Total Time"});  // [G]: TODO make const but where?
    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();  // [G]: Bit hacky couldnt do it by default
}
