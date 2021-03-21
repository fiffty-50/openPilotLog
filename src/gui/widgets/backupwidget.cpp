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


    model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels(QStringList{"Backup File","Total Flights", "Total Tails",
                                                 "Total Pilots", "Max Doft", "Total Time"});  // [G]: TODO make const but where?
    fillTableWithSampleData();
}

BackupWidget::~BackupWidget()
{
    delete ui;
}


void BackupWidget::on_tableView_clicked(const QModelIndex &index)
{
    selectedBackupName = model->item(index.row(), 0);
    DEB << "Item at row:" << index.row() << "->" << selectedBackupName->data(Qt::DisplayRole);
}

void BackupWidget::on_createLocalPushButton_clicked()
{
    NOT_IMPLEMENTED
    QString filename = QFileDialog::getSaveFileName(
                this,
                "Choose destination file",
                AStandardPaths::directory(AStandardPaths::Backup).absolutePath(),
                "*.db"
    );

    if(filename.endsWith(".db") == false) {  // [G]: clunky im sure it can be enforced by QFileDialog
        filename.append(".db");
    }

    if(aDB->createBackup(filename) == false) {
        WARN << "Could not create local file:" << filename;
    }

    // [G] TODO: propably make a function out of this for future tweaks
    QFileIconProvider provider;
    QMap<QString, QString> summary = aDB->databaseSummary(filename);
    model->appendRow({new QStandardItem(provider.icon(QFileIconProvider::File), QFileInfo(filename).baseName()),
                      new QStandardItem(summary["total_flights"]),
                      new QStandardItem(summary["total_tails"]),
                      new QStandardItem(summary["total_pilots"]),
                      new QStandardItem(summary["max_doft"]),
                      new QStandardItem(summary["total_time"])
                     });
}

void BackupWidget::on_restoreLocalPushButton_clicked()
{
    if(selectedBackupName == nullptr) {
        INFO << "No backup selected";
        return;
    }
    QString backup_name = selectedBackupName->data(Qt::DisplayRole).toString();
    if(aDB->restoreBackup(backup_name) == false) {
        WARN << "Couldnt restore" << backup_name;
    }
}

void BackupWidget::on_deleteSelectedPushButton_clicked()
{
    if(selectedBackupName == nullptr) {
        INFO << "No backup was selected";
        return;
    }
    DEB << "deleting:" << selectedBackupName->data(Qt::DisplayRole);
}

void BackupWidget::on_createExternalPushButton_clicked()
{
    NOT_IMPLEMENTED
    QString filename = QFileDialog::getSaveFileName(
                this,
                "Choose destination file",
                AStandardPaths::directory(AStandardPaths::Backup).absolutePath(),
                ".db"
    );
    // [G]: The window isn resizable and i cant easily debug the buttons (cant find them xD)
    // [G] TODO: get time to properly format filename
    // Open something like a QFileDialog and let the user choose where to save the backup
}

void BackupWidget::on_restoreExternalPushButton_clicked()
{
    NOT_IMPLEMENTED
    QString filename = QFileDialog::getSaveFileName(
                this,
                "Choose backup file",
                // [G] TODO: home is the debug directory. Will it be ~ correctly?
                // Qt docs say it is (Confirm debug exception)
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
    QFileIconProvider provider;

    // [G]: works but a bit too hardcoded perhaps? The aviation industry wont change overnight
    // but still it could be worthwile to at least have the names a bit more encapsulated in the
    // database so we have them more "centralised" at least.

    // Get summary of each db file and populate lists (columns) of data
    for (const auto &entry : entries) {
        QMap<QString, QString> summary = aDB->databaseSummary(backup_dir.absoluteFilePath(entry));

        model->appendRow({new QStandardItem(provider.icon(QFileIconProvider::File), entry),
                          new QStandardItem(summary["total_flights"]),
                          new QStandardItem(summary["total_tails"]),
                          new QStandardItem(summary["total_pilots"]),
                          new QStandardItem(summary["max_doft"]),
                          new QStandardItem(summary["total_time"])
                         });
    }

    // [G]: Sort entries? based on what? the files are abit inconsistent in their naming atm
    // but i assume we could sort based on the time in the file name?

    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();  // [G]: Bit hacky couldnt do it by default
}
