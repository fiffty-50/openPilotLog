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
#ifndef BACKUPWIDGET_H
#define BACKUPWIDGET_H


#include <QWidget>
#include <QStandardItemModel>
#include <QFileSystemModel>
#include <QFileSystemWatcher>
#include <QTableView>

namespace Ui {
class BackupWidget;
}

/*!
 * \brief The BackupWidget is the interface for the user to create and restore backups of the
 * database.
 * \details OpenPilotLog offers two kinds of backups: Local and External Backups.<br><br>Local backups
 * are automatically stored in a folder determined by OPL::Paths and automatically presented to the user in a List.
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

    /*!
     * \brief Generates a filename for creating a backup
     */
    static const QString backupName();

    /*!
     * \brief Generates the absolute path for a new local backup file.
     */
    static const QString absoluteBackupPath();

private slots:
    void on_tableView_clicked(const QModelIndex &index);

    /*!
     * \brief Creates a local Backup in the location determined by Paths::Backup
     */
    void on_createLocalPushButton_clicked();

    /*!
     * \brief Restores a backup from the folder determined by Paths::Backup
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
    void on_createNewLogbookPushButton_clicked();

private:
    Ui::BackupWidget *ui;

    QStandardItemModel *model;
    QTableView *view;
    QList<int> selectedRows;
    void refresh();

protected:
    /*!
     * \brief Handles change events, like updating the UI to new localisation
     */
    void changeEvent(QEvent* event) override;
};

#endif // BACKUPWIDGET_H
