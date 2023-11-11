/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2023 Felix Turowsky
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
#include "src/database/database.h"
#include "src/functions/datetime.h"
#include "src/database/dbsummary.h"
#include "src/gui/dialogues/firstrundialog.h"

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
    model->setHorizontalHeaderLabels(QStringList{tr("Total Time"),tr("Flights"), tr("Aircraft"),
                                                 tr("Pilots"), tr("Last Flight"), tr("Backup File")});
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
    QDir backup_dir = OPL::Paths::directory(OPL::Paths::Backup);
    const QStringList entries = backup_dir.entryList(QStringList{"*.db"}, QDir::Files, QDir::Time);
    QFileIconProvider provider;

    // Get summary of each db file and populate lists (columns) of data
    for (const auto &entry : entries) {
        QMap<OPL::DbSummaryKey, QString> summary = OPL::DbSummary::databaseSummary(backup_dir.absoluteFilePath(entry));
        model->appendRow({new QStandardItem(summary[OPL::DbSummaryKey::total_time]),
                          new QStandardItem(summary[OPL::DbSummaryKey::total_flights]),
                          new QStandardItem(summary[OPL::DbSummaryKey::total_tails]),
                          new QStandardItem(summary[OPL::DbSummaryKey::total_pilots]),
                          new QStandardItem(summary[OPL::DbSummaryKey::last_flight]),
                          new QStandardItem(provider.icon(QFileIconProvider::File), entry),
                         });
    }

    view->setModel(model);
    view->resizeColumnsToContents();
}

const QString BackupWidget::absoluteBackupPath()
{
    const QString backup_name = backupName();
    return OPL::Paths::filePath(OPL::Paths::Backup, backup_name);
}

const QString BackupWidget::backupName()
{
    auto owner = DB->getPilotEntry(1);
    return  QString("logbook_backup_%1_%2.db").arg(
        OPL::DateTime::dateTimeToString(QDateTime::currentDateTime(), OPL::DateTimeFormat_deprecated::Backup),
                                                  owner.getLastName()
                );
}

void BackupWidget::on_tableView_clicked(const QModelIndex &index)
{
    selectedRows.clear();
    selectedRows.append(index.row());
}

void BackupWidget::on_createLocalPushButton_clicked()
{
    QString filename = absoluteBackupPath();
    DEB << filename;

    if(!DB->createBackup(QDir::toNativeSeparators(filename))) {
        WARN(tr("Could not create local file: %1").arg(filename));
        return;
    } else {
        INFO(tr("Backup successfully created."));
    }

    QFileInfo file_info(filename);
    QFileIconProvider provider;
    QMap<OPL::DbSummaryKey, QString> summary = OPL::DbSummary::databaseSummary(filename);
    model->insertRow(0, {new QStandardItem(summary[OPL::DbSummaryKey::total_time]),
                         new QStandardItem(summary[OPL::DbSummaryKey::total_flights]),
                         new QStandardItem(summary[OPL::DbSummaryKey::total_tails]),
                         new QStandardItem(summary[OPL::DbSummaryKey::total_pilots]),
                         new QStandardItem(summary[OPL::DbSummaryKey::last_flight]),
                         new QStandardItem(provider.icon(QFileIconProvider::File), file_info.fileName()),
                     });
}

void BackupWidget::on_restoreLocalPushButton_clicked()
{
    if(selectedRows.isEmpty()) {
        INFO(tr("No backup selected"));
        return;
    }

    const QString file_name = model->item(selectedRows.first(), 5)->data(Qt::DisplayRole).toString();
    const QString backup_name = OPL::Paths::filePath(OPL::Paths::Backup, file_name);

    QMessageBox confirm(this);
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirm.setDefaultButton(QMessageBox::No);
    confirm.setIcon(QMessageBox::Warning);
    confirm.setWindowTitle(tr("Restoring Backup"));
    confirm.setText(tr("The following backup will be restored:<br><br><b><tt>"
                       "%1</b></tt><br><br>"
                       "This will replace your currently active database with the backup.<br>This action is irreversible.<br><br>Are you sure?"
                       ).arg(OPL::DbSummary::summaryString(backup_name)));
    if (confirm.exec() == QMessageBox::No)
        return;

    if(!DB->restoreBackup(QDir::toNativeSeparators(backup_name))) {
       WARN(tr("Unable to restore Backup file: %1").arg(backup_name));
    } else {
        INFO(tr("Backup successfully restored."));
    }

    view->clearSelection();
    selectedRows.clear();
}

void BackupWidget::on_deleteSelectedPushButton_clicked()
{
    if(selectedRows.isEmpty()) {
        INFO(tr("No backup was selected"));
        return;
    }

    const QString file_name = model->item(selectedRows.first(), 5)->data(Qt::DisplayRole).toString();
    const QString backup_name = OPL::Paths::filePath(OPL::Paths::Backup, file_name);
    QFile file(OPL::Paths::filePath(OPL::Paths::Backup, file_name));

    if(!file.exists()) {
        WARN(tr("Selected backup file (<tt>%1</tt>) does not exist.").arg(file_name));
        return;
    }

    QMessageBox confirm(this);
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirm.setDefaultButton(QMessageBox::No);
    confirm.setIcon(QMessageBox::Question);
    confirm.setWindowTitle(tr("Delete Backup"));
    confirm.setText(tr("The following backup will be deleted:<br><br><i>%1</i><br><br><b><tt>"
                       "%2</b></tt><br><br>"
                       "<b>This action is irreversible.</b><br><br>Continue?"
                       ).arg(file_name, OPL::DbSummary::summaryString(backup_name)));
    if (confirm.exec() == QMessageBox::No)
        return;

    LOG << "Deleting backup:" << file_name;
    if(!file.remove()) {
        WARN(tr("Unable to remove file %1<br>Error: %2").arg(file_name,file.errorString()));
        return;
    } else {
        INFO(tr("Backup successfully deleted."));
    }

    model->removeRow(selectedRows.first());
    view->clearSelection();
    selectedRows.clear();
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

    if(!DB->createBackup(QDir::toNativeSeparators(filename))) {
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
                       "<br>Continue?"
                       ).arg(OPL::DbSummary::summaryString(filename)));
    if (confirm.exec() == QMessageBox::Yes) {
        if(!DB->restoreBackup(QDir::toNativeSeparators(filename))) {
            WARN(tr("Unable to import database file:").arg(filename));
            return;
        }
        INFO(tr("Database successfully imported."));
    }
}

/*!
 * \brief BackupWidget::on_createNewLogbookPushButton_clicked Enables the user to reset the database
 */
void BackupWidget::on_createNewLogbookPushButton_clicked()
{
    QMessageBox confirm(this);
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirm.setDefaultButton(QMessageBox::No);
    confirm.setIcon(QMessageBox::Warning);
    confirm.setWindowTitle(tr("Start new Logbook"));
    confirm.setText(tr("By starting a new logbook, you reset and empty the database currently in use.<br><br>"
                       "You will be asked if you want to keep a backup of your current database, but it is highly "
                       "recommended to create an external backup before starting a new logbook.<br><br>"
                       "Do you want to continue?"
                       ));
    if (confirm.exec() == QMessageBox::Yes) {
        auto frd = new FirstRunDialog(this);
        if(!frd->exec()) {
            WARN(tr("Creating New Logbook has been unsuccessful or aborted."));
        }
    }

}
