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
    QStandardItem *selected;

    void fillTableWithSampleData();
};

#endif // BACKUPWIDGET_H
