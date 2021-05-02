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
#ifndef BACKUPWIDGET_H
#define BACKUPWIDGET_H

#include "src/classes/astandardpaths.h"

#include <QWidget>
#include <QStandardItemModel>
#include <QFileSystemModel>
#include <QFileSystemWatcher>
#include <QTableView>

namespace Ui {
class BackupWidget;
}

/*!
 * \brief Simple QStandardItem subclass to encapsulate necessary file info.
 * Using only a QStandardItem would mean that the full path should be inputted
 * as data and of course displayed by default. However this way we create
 * the absolute path in the fileInfo attribute for further use while
 * displaying only the base name.
 */
class AFileStandardItem : public QStandardItem {
private:
    QFileInfo fileInfo;
public:
    AFileStandardItem(const QIcon& icon, const QString& filename, const AStandardPaths::Directories dir)
        : QStandardItem(icon, filename),
          fileInfo(QFileInfo(AStandardPaths::asChildOfDir(dir, filename)))
    {}
    AFileStandardItem(const QIcon& icon, const QFileInfo file_info)
        : QStandardItem(icon, file_info.baseName()),
          fileInfo(QFileInfo(file_info))
    {}


    const QFileInfo& info() const
    {
        return fileInfo;
    }
};

/*!
 * \brief The BackupWidget is the interface for the user to create and restore backups of the
 * database.
 * \details OpenPilotLog offers two kinds of backups: Local and External Backups.<br><br>Local backups
 * are automatically stored in a folder determined by AStandardpaths and automatically presented to the user in a List.
 * <b>Create Local backup</b> and <b>Restore Local Backup</b>. are convenient shortcuts.<br>
 * When using <b>Create External Backup</b>, the user will be asked where to save the backup file. This can be a pen drive, a cloud location or any other location of his choice.
 * This functionality can also be used to sync the database across devices. External backup files con be restored with <b>Restore external backup</b>.
 */
class BackupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BackupWidget(QWidget *parent = nullptr);
    ~BackupWidget();

private slots:
    void on_tableView_clicked(const QModelIndex &index);

    /*!
     * \brief Creates a local Backup in the location determined by AStandardPaths::Backup
     */
    void on_createLocalPushButton_clicked();

    /*!
     * \brief Restores a backup from the folder determined by AStandardPaths::Backup
     */
    void on_restoreLocalPushButton_clicked();

    /*!
     * \brief Deletes the selected backup from the local backup directory based on the user selection in the QTableView
     */
    void on_deleteSelectedPushButton_clicked();

    /*!
     * \brief opens a QFileDialog to allow the user to select a location to save the backup to
     */
    void on_createExternalPushButton_clicked();

    /*!
     * \brief opens a QFileDialog to allow the user to restore a backup from a chosen location
     */
    void on_restoreExternalPushButton_clicked();

    /*!
     * \brief Opens a QMessageBox that outlines the different functions of the Backupwidget to the user.
     */
    void on_aboutPushButton_clicked();

private:
    Ui::BackupWidget *ui;

    QStandardItemModel *model;
    QTableView *view;
    AFileStandardItem *selectedFileInfo = nullptr;  // Only the first column is necessary for
                                                    // any operation and it is encapsulated in the
                                                    // AFileStandardItem class
    void refresh();

    /*!
     * \brief Generates a filename for creating a backup
     */
    const QString backupName();

    /*!
     * \brief Generates the absolute path for a new backup file.
     */
    const QString absoluteBackupPath();
};

#endif // BACKUPWIDGET_H
