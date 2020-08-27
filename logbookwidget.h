#ifndef LOGBOOKWIDGET_H
#define LOGBOOKWIDGET_H

#include <QWidget>
#include <QItemSelection>

namespace Ui {
class logbookWidget;
}

class logbookWidget : public QWidget
{
    Q_OBJECT

public:
    explicit logbookWidget(QWidget *parent = nullptr);
    ~logbookWidget();

private slots:
    void on_newFlightButton_clicked();

    void on_editFlightButton_clicked();

    void on_deleteFlightPushButton_clicked();

    void on_filterFlightsByDateButton_clicked();

    void on_showAllButton_clicked();

    void on_tableView_entered(const QModelIndex &index);

    void on_tableView_pressed(const QModelIndex &index);

    void on_tableView_changed(const QItemSelection &index, const QItemSelection &index2);

private:
    Ui::logbookWidget *ui;
};

#endif // LOGBOOKWIDGET_H
