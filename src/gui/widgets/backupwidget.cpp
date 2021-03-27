#include "backupwidget.h"
#include "ui_backupwidget.h"
#include "src/classes/astandardpaths.h"
#include "src/testing/adebug.h"
#include "src/database/adatabase.h"
#include "src/functions/adatetime.h"

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
    refresh();
}

BackupWidget::~BackupWidget()
{
    delete ui;
}

QString BackupWidget::absoluteBackupPath()
{
    const QString backup_name = QLatin1String("logbook_backup_")
            + ADateTime::toString(QDateTime::currentDateTime(), Opl::Datetime::Backup)
            + QLatin1String(".db");
    return AStandardPaths::asChildOfDir(AStandardPaths::Backup, backup_name);
}

void BackupWidget::on_tableView_clicked(const QModelIndex &index)
{
    selectedFileInfo = static_cast<AFileStandardItem*>(model->item(index.row(), 0));
    DEB << "Item at row:" << index.row() << "->" << selectedFileInfo->data(Qt::DisplayRole);
}

void BackupWidget::on_createLocalPushButton_clicked()
{
    QString filename = absoluteBackupPath();
    DEB << filename;

    if(!aDB->createBackup(filename)) {
        WARN << "Could not create local file:" << filename;
        return;
    }

    // [G] TODO: propably make a function out of this for future tweaks
    QFileIconProvider provider;
    QMap<ADatabaseSummaryKey, QString> summary = aDB->databaseSummary(filename);
    model->appendRow({new AFileStandardItem(provider.icon(QFileIconProvider::File), QFileInfo(filename)),
                      new QStandardItem(summary[ADatabaseSummaryKey::total_flights]),
                      new QStandardItem(summary[ADatabaseSummaryKey::total_tails]),
                      new QStandardItem(summary[ADatabaseSummaryKey::total_pilots]),
                      new QStandardItem(summary[ADatabaseSummaryKey::max_doft]),
                      new QStandardItem(summary[ADatabaseSummaryKey::total_time])
                     });
}

void BackupWidget::on_restoreLocalPushButton_clicked()
{
    NOT_IMPLEMENTED("TODO");

    if(selectedFileInfo == nullptr) {
        INFO << "No backup selected";
        return;
    }

    QString backup_name = AStandardPaths::asChildOfDir(
                AStandardPaths::Backup,
                selectedFileInfo->data(Qt::DisplayRole).toString()
                );

    if(!aDB->restoreBackup(backup_name)) {
        WARN << "Couldnt restore" << backup_name;
    }
}

void BackupWidget::on_deleteSelectedPushButton_clicked()
{
    NOT_IMPLEMENTED("Test external deletion");
    if(selectedFileInfo == nullptr) {
        INFO << "No backup was selected";
        return;
    }

    const QFileInfo& filename = selectedFileInfo->info();
    QFile file(filename.absoluteFilePath());

    if(file.exists() == false) {
        WARN << "Selected backup file doesnt exist:" << file;
        return;
    }

    DEB << "deleting:" << filename;
    if(file.remove() == false) {
        WARN << "Unable to remove file:" << file.errorString();
        return;
    }

    model->removeRow(selectedFileInfo->row());
    // [G] TODO: figure out selection coordination between view model and selected
    if(selectedFileInfo->row() - 1 < 0) {
        selectedFileInfo = nullptr;
    }
    else {
        selectedFileInfo = static_cast<AFileStandardItem*>(model->item(selectedFileInfo->row()-1));
    }
}

void BackupWidget::on_createExternalPushButton_clicked()
{
    NOT_IMPLEMENTED("TODO");
    QString filename = QFileDialog::getSaveFileName(
                this,
                "Choose destination file",
                QStandardPaths::displayName(QStandardPaths::HomeLocation),
                ".db"
    );
    // [G]: The window isn resizable and i cant easily debug the buttons (cant find them xD)
    // Open something like a QFileDialog and let the user choose where to save the backup
}

void BackupWidget::on_restoreExternalPushButton_clicked()
{
    NOT_IMPLEMENTED("TODO");
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
    // [G]: Add message text. Could this be predefined in Opl::Assets?
    QMessageBox msg_box(QMessageBox::Information, "About backups", "...", QMessageBox::Ok);
    msg_box.exec();
}



// ===================================================================================

// feel free to delete this as you go along, just here for demonstration purposes
void BackupWidget::refresh()
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
        QMap<ADatabaseSummaryKey, QString> summary = aDB->databaseSummary(backup_dir.absoluteFilePath(entry));
        model->appendRow({new AFileStandardItem(provider.icon(QFileIconProvider::File), entry, AStandardPaths::Backup),
                          new QStandardItem(summary[ADatabaseSummaryKey::total_flights]),
                          new QStandardItem(summary[ADatabaseSummaryKey::total_tails]),
                          new QStandardItem(summary[ADatabaseSummaryKey::total_pilots]),
                          new QStandardItem(summary[ADatabaseSummaryKey::max_doft]),
                          new QStandardItem(summary[ADatabaseSummaryKey::total_time])
                         });
    }

    // [G]: Sort entries? based on what? the files are abit inconsistent in their naming atm
    // but i assume we could sort based on the time in the file name?

    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();  // [G]: Bit hacky couldnt do it by default
}
