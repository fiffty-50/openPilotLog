#include "backupwidget.h"
#include "ui_backupwidget.h"
#include "src/classes/astandardpaths.h"
#include "src/testing/adebug.h"

#include <QListView>
#include <QStandardItemModel>
#include <QFileIconProvider>

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
    // Remove selected backup from list and delete file.
}

void BackupWidget::on_createExternalPushButton_clicked()
{
    // Open something like a QFileDialog and let the user choose where to save the backup
}

void BackupWidget::on_restoreExternalPushButton_clicked()
{
    // Open something like a QFileDialog and let the user choose where to load the backup from
}

void BackupWidget::on_aboutPushButton_clicked()
{
    // Shows a message box explaining a little about local and external backups
}



// ===================================================================================

// feel free to delete this as you go along, just here for demonstration purposes
void BackupWidget::fillTableWithSampleData()
{
    // First column in table, would be created by listing the files in backupdir
    QDir backup_dir = QDir(AStandardPaths::directory(AStandardPaths::Backup));
    QStringList entries = backup_dir.entryList(QStringList{"*.db"});  // [G]: TODO make const but where?
    QList<QStandardItem*> filenames;
    QFileIconProvider provider;

    for(auto& entry : entries) {
        auto item = new QStandardItem(entry);
        item->setIcon(provider.icon(QFileIconProvider::File));
        filenames.append(item);
    }

    // TODO
    // Second column, would be created by reading the details from each file and creating the description string
    QList<QStandardItem *> descriptions{};
    QStringList descriptions_string_list {
        "2020-02-12 - 512 Flights, 25 Aircraft, 44 Pilots, 1640:47 Total Time, Last Flight 2020-01-23",
        "2020-01-15 - 476 Flights, 24 Aircraft, 42 Pilots, 1490:23 Total Time, Last Flight 2019-12-06",
        "2020-12-23 - 452 Flights, 19 Aircraft, 39 Pilots, 1460:34 Total Time, Last Flight 2019-11-23",
        "2020-11-21 - 435 Flights, 17 Aircraft, 33 Pilots, 1373:25 Total Time, Last Flight 2019-10-16",
        "2020-10-30 - 419 Flights, 15 Aircraft, 12 Pilots, 1337:02 Total Time, Last Flight 2019-09-24",
    };
    for (const auto &string : descriptions_string_list) {
        auto item = new QStandardItem(string);
        descriptions.append(item);
    }

    model = new QStandardItemModel(this);
    model->insertColumn(0, filenames);
    model->insertColumn(1, descriptions);
    model->setHorizontalHeaderLabels(QStringList{"Backup File","Summary"});  // [G]: TODO make const but where?
    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();  // [G]: Bit hacky couldnt do it by default
}
