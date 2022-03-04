/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2021 Felix Turowsky
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
#include "backupwidget.h"
#include "ui_backupwidget.h"
#include "src/opl.h"
#include "src/classes/astandardpaths.h"
#include "src/functions/alog.h"
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

    model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels(QStringList{tr("Backup File"),tr("Flights"), tr("Aircraft"),
                                                 tr("Pilots"), tr("Last Flight"), tr("Total Time")});
    view = ui->tableView;
    refresh();
}

BackupWidget::~BackupWidget()
{
    delete ui;
}

void BackupWidget::changeEvent(QEvent *event)
{
    if (event != nullptr)
        if(event->type() == QEvent::LanguageChange)
            ui->retranslateUi(this);
}

void BackupWidget::refresh()
{
    // First column in table, would be created by listing the files in backupdir
    QDir backup_dir = QDir(AStandardPaths::directory(AStandardPaths::Backup));
    const QStringList entries = backup_dir.entryList(QStringList{"*.db"}, QDir::Files, QDir::Time);
    QFileIconProvider provider;

    // Get summary of each db file and populate lists (columns) of data
    for (const auto &entry : entries) {
        QMap<ADatabaseSummaryKey, QString> summary = aDB->databaseSummary(backup_dir.absoluteFilePath(entry));
        model->appendRow({new AFileStandardItem(provider.icon(QFileIconProvider::File), entry, AStandardPaths::Backup),
                          new QStandardItem(summary[ADatabaseSummaryKey::total_flights]),
                          new QStandardItem(summary[ADatabaseSummaryKey::total_tails]),
                          new QStandardItem(summary[ADatabaseSummaryKey::total_pilots]),
                          new QStandardItem(summary[ADatabaseSummaryKey::last_flight]),
                          new QStandardItem(summary[ADatabaseSummaryKey::total_time])
                         });
    }

    view->setModel(model);
    view->resizeColumnsToContents();
}

const QString BackupWidget::absoluteBackupPath()
{
    const QString backup_name = QLatin1String("logbook_backup_")
            + ADateTime::toString(QDateTime::currentDateTime(), Opl::Datetime::Backup)
            + QLatin1String(".db");
    return AStandardPaths::asChildOfDir(AStandardPaths::Backup, backup_name);
}

const QString BackupWidget::backupName()
{
    return  QLatin1String("logbook_backup_")
            + ADateTime::toString(QDateTime::currentDateTime(), Opl::Datetime::Backup)
            + QLatin1String(".db");
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
        WARN(tr("Could not create local file: %1").arg(filename));
        return;
    } else {
        INFO(tr("Backup successfully created."));
    }

    QFileIconProvider provider;
    QMap<ADatabaseSummaryKey, QString> summary = aDB->databaseSummary(filename);
    model->insertRow(0, {new AFileStandardItem(provider.icon(QFileIconProvider::File), QFileInfo(filename)),
                      new QStandardItem(summary[ADatabaseSummaryKey::total_flights]),
                      new QStandardItem(summary[ADatabaseSummaryKey::total_tails]),
                      new QStandardItem(summary[ADatabaseSummaryKey::total_pilots]),
                      new QStandardItem(summary[ADatabaseSummaryKey::last_flight]),
                      new QStandardItem(summary[ADatabaseSummaryKey::total_time])
                     });
}

void BackupWidget::on_restoreLocalPushButton_clicked()
{
    if(selectedFileInfo == nullptr) {
        INFO(tr("No backup selected"));
        return;
    }

    QString backup_name = AStandardPaths::asChildOfDir(
                AStandardPaths::Backup,
                selectedFileInfo->data(Qt::DisplayRole).toString()
                );

    QMessageBox confirm(this);
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirm.setDefaultButton(QMessageBox::No);
    confirm.setIcon(QMessageBox::Warning);
    confirm.setWindowTitle(tr("Restoring Backup"));
    confirm.setText(tr("The following backup will be restored:<br><br><b><tt>"
                       "%1</b></tt><br><br>"
                       "This will replace your currently active database with the backup.<br>This action is irreversible.<br><br>Are you sure?"
                       ).arg(aDB->databaseSummaryString(backup_name)));
    if (confirm.exec() == QMessageBox::No)
        return;

    if(!aDB->restoreBackup(backup_name)) {
       WARN(tr("Unable to restore Backup file: %1").arg(backup_name));
    } else {
        INFO(tr("Backup successfully restored."));
    }

    view->clearSelection();
    selectedFileInfo = nullptr;
}

void BackupWidget::on_deleteSelectedPushButton_clicked()
{
    if(selectedFileInfo == nullptr) {
        INFO(tr("No backup was selected"));
        return;
    }

    const QFileInfo& filename = selectedFileInfo->info();
    QFile file(filename.absoluteFilePath());

    if(!file.exists()) {
        WARN(tr("Selected backup file (<tt>%1</tt>) does not exist.").arg(filename.absolutePath()));
        return;
    }

    QMessageBox confirm(this);
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirm.setDefaultButton(QMessageBox::No);
    confirm.setIcon(QMessageBox::Question);
    confirm.setWindowTitle(tr("Delete Backup"));
    confirm.setText(tr("The following backup will be deleted:<br><br><b><tt>"
                       "%1</b></tt><br><br>"
                       "Are you sure?"
                       ).arg(filename.fileName()));
    if (confirm.exec() == QMessageBox::No)
        return;

    LOG << "Deleting backup:" << filename;
    if(!file.remove()) {
        WARN(tr("Unable to remove file %1\nError: %2").arg(filename.fileName(),file.errorString()));
        return;
    } else {
        INFO(tr("Backup successfully deleted."));
    }

    model->removeRow(selectedFileInfo->row());
    view->clearSelection();
    selectedFileInfo = nullptr;
}

void BackupWidget::on_createExternalPushButton_clicked()
{
    QString filename = QFileDialog::getSaveFileName(
                this,
                tr("Choose destination file"),
                QDir::homePath() + QDir::separator() + backupName(),
                "*.db"
    );

    if(filename.isEmpty()) { // QFileDialog has been cancelled
        return;
    }

    if(!filename.endsWith(".db")) {
        filename.append(".db");
    }

    if(!aDB->createBackup(filename)) {
        WARN(tr("Unable to backup file:").arg(filename));
        return;
    } else {
        INFO(tr("Backup successfully created."));
    }
}

void BackupWidget::on_restoreExternalPushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(
                this,
                tr("Choose backup file"),
                QDir::homePath(),
                "*.db"
    );

    if(filename.isEmpty()) { // QFileDialog has been cancelled
        return;
    }

    // Maybe create a Message Box asking for confirmation here and displaying the summary of backup and active DB

    QMessageBox confirm(this);
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirm.setDefaultButton(QMessageBox::No);
    confirm.setIcon(QMessageBox::Question);
    confirm.setWindowTitle(tr("Import Database"));
    confirm.setText(tr("The following database will be imported:<br><br><b><tt>"
                       "%1<br></b></tt>"
                       "<br>Is this correct?"
                       ).arg(aDB->databaseSummaryString(filename)));
    if (confirm.exec() == QMessageBox::Yes) {
        if(!aDB->restoreBackup(filename)) {
            WARN(tr("Unable to import database file:").arg(filename));
            return;
        }
        INFO(tr("Database successfully imported."));
    }
}

void BackupWidget::on_aboutPushButton_clicked()
{
    TODO << "Implement settings and automatic backups";
    QString text = tr(

                      "<h3><center>About Backups</center></h3>"
                      "<br>"
                      "<p>By creating a backup, you create a copy of your logbook for safekeeping. This copy includes all your "
                      "flights, pilots, aircraft and expiries. By creating a backup, you are creating a snapshot of your logbook to date. This backup can "
                      "later be restored. OpenPilotLog offers two kinds of backups: Local and External Backups.<br><br>Local backups "
                      "are automatically stored in a folder on this computer and will show up in the list below. They can easily be created by selecting <b>Create Local backup</b> and restored with "
                      "<b>Restore Local Backup</b>.<br>"
                      "When using <b>Create External Backup</b>, you will be asked where to save your backup file. This can be a pen drive, a cloud location or any other location of your choice. "
                      "This functionality can also be used to sync your database across devices or to take it with you when you buy a new PC. You can then import your backup file by selecting "
                      "it with <b>Restore external backup</b>.</p>"
                      "<p>Frequent backups are recommended to guard against data loss or corruption. It is also recommended to keep a backup copy in a seperate location from your main "
                      "computer to prevent data loss due to system failures.</p>"
                      //todo "<p>By default, OpenPilotLog creates a weekly automatic backup. If you would like to change this behaviour, you can adjust it in the settings.</p>"
                      "<br>"
                      );
    QMessageBox msg_box(QMessageBox::Information, "About backups", text, QMessageBox::Ok, this);
    msg_box.exec();
}
