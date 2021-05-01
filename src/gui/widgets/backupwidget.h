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
 * \details To Do...
 */
class BackupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BackupWidget(QWidget *parent = nullptr);
    ~BackupWidget();

private slots:
    void on_tableView_clicked(const QModelIndex &index);

    void on_createLocalPushButton_clicked();

    void on_restoreLocalPushButton_clicked();

    void on_deleteSelectedPushButton_clicked();

    void on_createExternalPushButton_clicked();

    void on_restoreExternalPushButton_clicked();

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
